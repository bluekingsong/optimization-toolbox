import cv2
import numpy as np
from paddleocr import PaddleOCR
from ppocr.utils.logging import get_logger
import logging
from datetime import datetime
logger = get_logger()
logger.setLevel(logging.ERROR)
import os
from moviepy.editor import VideoFileClip, concatenate_videoclips
import sys

ts_file_fmt = 'bwf_world_tour_finals_2023/ts_files/v.f721217_%d.ts'
ts_file_list = [ (x, ts_file_fmt%(x)) for x in range(1144, 2267) ]
start_rally_cnt = 612
rally_prefix = 'bwf_world_tour_finals_2023/rallys/wtf2023'
def get_score(result, latest_score, hint):
    score, new_hint = None, None
    if result is not None:
        if len(result) >= 4 and len(result) % 2 == 0:
            num_match = (len(result) - 2) // 2
            if result[num_match][1][0].isdigit() and result[num_match + 1 + num_match][1][0].isdigit():
                player1, player2 = result[0][1][0].replace(' ', ''), result[num_match + 1][1][0].replace(' ', '')
                new_hint = (num_match, player1, player2)
                score = (int(result[num_match][1][0]), int(result[num_match + 1 + num_match][1][0]))
                if any(char.isdigit() for char in player1) or any(char.isdigit() for char in player2):   
                    new_hint = None
                    score = None
                if latest_score is not None and score is not None:
                    if score[0] not in (latest_score[0], latest_score[0] + 1) or score[1] not in (latest_score[1], latest_score[1] + 1):
                        print('drop score=%s latest_score=%s'%(score, latest_score))
                        score = None  # not confidence
        elif hint is not None:
            num_match, player1, player2 = hint
            score1, score2 = None, None
            for i in range(len(result)):
                if result[i][1][0].replace(' ', '') == player1:
                    if i + num_match < len(result) and all(result[i + j][1][0].isdigit() for j in range(1, num_match+1)):   score1 = int(result[i + num_match][1][0])
                if result[i][1][0].replace(' ', '') == player2:
                    if i + num_match < len(result) and all(result[i + j][1][0].isdigit() for j in range(1, num_match+1)):   score2 = int(result[i + num_match][1][0])
            if score1 is not None and score2 is not None:
                score = (score1, score2)
                if latest_score is not None:
                    if score[0] not in (latest_score[0], latest_score[0] + 1) or score[1] not in (latest_score[1], latest_score[1] + 1):
                        print('drop hint score=%s latest_score=%s'%(score, latest_score))
                        score = None  # not confidence
    if new_hint is None:   new_hint = hint
    return score, new_hint

def rally_detect(ts_file_list, rally_cnt = 0):
    ocr = PaddleOCR(use_angle_cls=False, lang="en", drop_score=0.3)  # need to run only once to download and load model into memory
    tx, ty = 500, 160
    latest_score, hint = None, None
    rally = None
    rally_filename = None
    score_trace = list()
    for ts_id, ts_filename in ts_file_list:
        cap = cv2.VideoCapture(ts_filename)
        frame_cnt = cap.get(cv2.CAP_PROP_FRAME_COUNT)
        print('process file: ', ts_filename, 'num_frames=', frame_cnt, datetime.now())
        frame_width = int(cap.get(3)) 
        frame_height = int(cap.get(4)) 
        size = (frame_width, frame_height) 
        flag = True
        frame_index = 0
        while flag:
            flag, frame = cap.read()
            frame_index += 1
            if not flag:   break
            if frame_index % 5 != 0:  continue
            box_frame = frame[0:ty, 0:tx, :]
            result = ocr.ocr(box_frame, cls=False)[0]
            filter_result = list()
            for box_result in (result if result is not None else list()):
                if len(box_result) == 2:
                    box_info = np.array(box_result[0], np.int32)
                else:
                    box_info = np.array(box_result, np.int32)
                xmin, xmax, ymin, ymax = np.min(box_info[:, 0]), np.max(box_info[:, 0]), np.min(box_info[:, 1]), np.max(box_info[:, 1])
                if (xmax - xmin) <= 10 or (ymax - ymin) <= 10:   continue
                filter_result.append(box_result)
            result = filter_result
            if result is not None:
                score, hint = get_score(filter_result, latest_score, hint)
                score_trace.append(score)
            else:
                score_trace.append('NoBox')
            if rally is not None and score_trace[-1] != 'NoBox':
                if ts_id not in rally:
                    rally[ts_id] = (frame_cnt, frame_index, frame_index)
                else:
                    rally[ts_id] = (rally[ts_id][0], rally[ts_id][1], frame_index)  
            if score is not None:
                if score != latest_score:
                    for line in result:   
                        box_info = np.array(line[0], np.int32)
                        xmin, xmax, ymin, ymax = np.min(box_info[:, 0]), np.max(box_info[:, 0]), np.min(box_info[:, 1]), np.max(box_info[:, 1])
                        cv2.rectangle(frame, (xmin, ymin), (xmax, ymax), (0,0,255), 3)
                    print('ScoreChange from %s to %s at file/frame=%s/%d, rally_file=%s'%(latest_score, score, ts_id, frame_index, rally_filename))
                    if rally_filename is not None:
                        print('generate rally ', rally_cnt, rally_filename, rally)
                        print('score_trace=', score_trace)
                        write_rally(rally, rally_filename)
                        sub_clips, clips = list(), list()
                    rally = {ts_id : (frame_cnt, frame_index, frame_index)}
                    rally_cnt += 1
                    score_trace = list()
                    rally_filename = '%s_%03d_%d_s_%d_%d.mp4'%(rally_prefix, rally_cnt, ts_id, score[0], score[1])
                    #rally_writer = cv2.VideoWriter(rally_filename,  cv2.VideoWriter_fourcc(*'MJPG'), 30, size)
                    timeout = 500
                latest_score = score
                if max(score) >= 21 and abs(score[0] - score[1]) >= 2:
                    latest_score = None
                    rally = None
                    rally_filename = None

            #cv2.imshow('%s/%d'%(ts_filename, frame_index),frame)
            #cv2.waitKey(timeout)
        cap.release()
    #if rally_writer:  rally_writer.release()

def write_rally(rally, rally_filename):
    sub_clips, clips = list(), list()
    for ts_id in sorted(rally.keys()):
        clip = VideoFileClip(ts_file_fmt%(ts_id))
        cnt, start, end = rally[ts_id]
        if start >= end:    continue
        #if (ts_id + 1) in rally:   end = cnt
        ts, te = start/clip.fps, min(clip.duration, end/clip.fps)
        try:
            sub_clip = clip.subclip(ts, te)
        except Exception as err:
            print('subclip, try to change file fmt:', ts_file_fmt%(ts_id), ts, te)
            if not os.path.exists(ts_file_fmt%(ts_id) + '.mp4'):
                os.system('ffmpeg -nostdin -i %s -c:v copy -c:a copy %s'%(ts_file_fmt%(ts_id), ts_file_fmt%(ts_id) + '.mp4'))
            mp4_clip = VideoFileClip(ts_file_fmt%(ts_id) + '.mp4')
            clips.append(mp4_clip)
            ts, te = start/mp4_clip.fps, min(clip.duration, end/mp4_clip.fps)
            sub_clip = mp4_clip.subclip(ts, te)
        sub_clips.append(sub_clip)
        clips.append(clip)
    concat_clip = concatenate_videoclips(sub_clips)
    concat_clip.write_videofile(rally_filename, audio_codec='aac', threads=8, logger=None)
    for clip in clips:   clip.close()
    concat_clip.close()

rally_detect(ts_file_list, start_rally_cnt)

def t():
    ts_filename = 'bwf_world_tour_finals_2023/ts_files/v.f721217_1144.ts'
    cap = cv2.VideoCapture(ts_filename)
    frame_cnt = cap.get(cv2.CAP_PROP_FRAME_COUNT)
    ocr = PaddleOCR(use_angle_cls=False, lang="en", drop_score=0.3)  # need to run only once to download and load model into memory
    tx, ty = 500, 160
    flag = True
    frame_index = 0
    latest_score, hint = None, None
    while flag:
        flag, frame = cap.read()
        frame_index += 1
        if not flag:   break
        if frame_index % 5 != 0:  continue
        #if ((latest_score is None or max(latest_score) < 20) and frame_index % 5 != 0) or (latest_score is not None and max(latest_score) >= 20 and frame_index % 2 != 0):  continue
        box_frame = frame[0:ty, 0:tx, :]
        result = ocr.ocr(box_frame, cls=False)[0]
        if result:
            score, hint = get_score(result, latest_score, hint)
            print('frame_index=', frame_index, latest_score, score, hint,  ['%s'%(x[1][0]) for x in result])
            if score:   latest_score = score

#t()
