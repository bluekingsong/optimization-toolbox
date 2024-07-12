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
os.environ['HF_ENDPOINT'] = 'https://hf-mirror.com'
import torch
import laion_clap
model = laion_clap.CLAP_Module(enable_fusion=False)
model.load_ckpt() # download the default pretrained checkpoint.

def split_rally(rally_filename, output_path='rally_splits'):
    num_split = 0
    clip = VideoFileClip(rally_filename)
    if clip.duration < 10:
        print('[WARN] skip too short clip:', rally_filename, clip.duration)
    else:
        audio_files = list()
        for s in range(0, int(clip.duration), 5):
            #if min(clip.duration, s + 5) < 3:  continue
            ts, te = s, min(clip.duration, s + 5)
            sub_clip = clip.subclip(ts, te)
            output_video_file = output_path + '/' + rally_filename.split('/')[-1] + '_%d_split.mp4'%(num_split)
            output_audio_file = output_path + '/' + rally_filename.split('/')[-1] + '_%d_split.aac'%(num_split)
            if not os.path.exists(output_video_file):
                sub_clip.write_videofile(output_video_file, audio_codec='aac', threads=8, logger=None)
            if not os.path.exists(output_audio_file):
                os.system('ffmpeg -nostdin -i %s -vn -acodec copy %s'%(output_video_file, output_audio_file))
            audio_files.append(output_audio_file)
            num_split += 1
        audio_embed = model.get_audio_embedding_from_filelist(x = audio_files, use_tensor=True)
        for i, audio_filename in enumerate(audio_files):
            torch.save(audio_embed[i],audio_filename.replace('split.aac', 'split.bin'))

    clip.close()
    return num_split

files = os.listdir('rallys')

for filename in files:
    print('Processing file: ', filename, datetime.now())
    split_rally('rallys/' + filename)
