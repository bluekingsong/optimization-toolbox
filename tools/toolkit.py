from paddleocr import PaddleOCR, draw_ocr
import cv2
import os
import sys
from ppocr.utils.logging import get_logger
import logging
from datetime import datetime
logger = get_logger()
logger.setLevel(logging.ERROR)
import numpy as np
from moviepy.editor import VideoFileClip
import torch


def test_audio():

    # Directly get audio embeddings from audio files
    audio_file = [  'debug.aac'  ]
    audio_embed = model.get_audio_embedding_from_filelist(x = audio_file, use_tensor=True)
    
    print(audio_embed.shape)

def cut_clip(rally_filename, t_vec, output_path='highlights'):
    clip = VideoFileClip(rally_filename)
    sub_clip_se_vec = t_vec
    for s, e in sub_clip_se_vec:
        sub_clip = clip.subclip(s, e)
        output_video_file = output_path + '/' + rally_filename.split('/')[-1].replace('.mp4', '') + '_t%d_%d.mp4'%(s, e)
        output_audio_file = output_path + '/' + rally_filename.split('/')[-1].replace('.mp4', '') + '_t%d_%d.aac'%(s, e)
        if not os.path.exists(output_video_file):
            sub_clip.write_videofile(output_video_file, audio_codec='aac', threads=8, logger=None)
        if not os.path.exists(output_audio_file):
            os.system('ffmpeg -nostdin -i %s -vn -acodec copy %s 1>/dev/null 2>/dev/null'%(output_video_file, output_audio_file))
            os.system('ffmpeg -nostdin -i %s -vcodec copy -acodec pcm_s16le -ar 16000 -ab 128k -ac 1 %s 1>/dev/null 2>/dev/null'%(output_audio_file, output_audio_file.replace('.aac', '.wav')))
            os.system('rm -f %s'%(output_audio_file))

def cut_highlights():
    clip_configs = '''rallys/rally_058_192_s_9_8.mp4 15-20
    rallys/rally_113_457_s_15_10.mp4 37-42
    rallys/rally_142_573_s_9_3.mp4 75-80
    rallys/rally_210_910_s_20_18.mp4 40-45
    rallys/rally_296_1211_s_19_10.mp4 30-35
    rallys/rally_299_1228_s_20_12.mp4 24-29
    rallys/rally_420_1821_s_16_15.mp4 34-39
    rallys/rally_427_1848_s_19_19.mp4 35-40
    rallys/rally_460_1996_s_3_4.mp4 30-35
    rallys/rally_469_2021_s_9_7.mp4 26-31
    rallys/rally_492_2096_s_20_19.mp4 37-42,42-47
    rallys/rally_517_2160_s_10_8.mp4 26-31'''
    rally_files = [x.strip().split(' ')[0] for x in clip_configs.split('\n')]
    clip_time_vecs = [x.strip().split(' ')[1].split(',') for x in clip_configs.split('\n')]
    for filename, t_vec in zip(rally_files, clip_time_vecs):
        t_vec = [(int(x.split('-')[0]), int(x.split('-')[1])) for x in t_vec]
        cut_clip(filename, t_vec)
        print(filename, 'processed')

#cut_highlights()

def generate_wav_files(output_path='wav_files'):
    files = ['rallys/' + x for x in os.listdir('rallys')]
    for filename in files:
        output_audio_file = output_path + '/' + filename.split('/')[-1].replace('.mp4', '.aac')
        if not os.path.exists(output_audio_file):
            os.system('ffmpeg -nostdin -i %s -vn -acodec copy %s 1>/dev/null 2>/dev/null'%(filename, output_audio_file))
            os.system('ffmpeg -nostdin -i %s -vcodec copy -acodec pcm_s16le -ar 16000 -ab 128k -ac 1 %s 1>/dev/null 2>/dev/null'%(output_audio_file, output_audio_file.replace('.aac', '.wav')))
            os.system('rm -f %s'%(output_audio_file))
        print(filename, 'processed')

generate_wav_files()

def wav2fbank(filename):
    import torchaudio
    from torchaudio.compliance import kaldi
    MELBINS=128
    TARGET_LEN=1024
    waveform, sr = torchaudio.load(filename)
    waveform = waveform - waveform.mean()

    # 498 128
    fbank = kaldi.fbank(waveform, htk_compat=True, sample_frequency=sr, use_energy=False, 
                        window_type='hanning', num_mel_bins=MELBINS, dither=0.0, frame_shift=10)
    # AudioSet: 1024 (16K sr)
    # ESC: 512 (8K sr)
    n_frames = fbank.shape[0]
    p = TARGET_LEN - n_frames
    # cut and pad
    if p > 0:
        m = torch.nn.ZeroPad2d((0, 0, 0, p))
        fbank = m(fbank)
    elif p < 0:
        fbank = fbank[0:TARGET_LEN, :]
    return fbank
def norm_fbank(fbank):
    norm_mean= -4.2677393
    norm_std= 4.5689974
    fbank = (fbank - norm_mean) / (norm_std * 2)
    return fbank

def visualize_audio():
    import matplotlib.pyplot as plt
    fbank = wav2fbank('debug3.wav')
    fbank = norm_fbank(fbank)
    plt.figure(figsize=(20, 4))
    plt.imshow(20*fbank.T.numpy(), origin='lower', interpolation='nearest', vmax=None, vmin=None,  aspect='auto')
    plt.savefig('debug3.jpg')

def highlight_judge(highlight_sample_emb, rally_filename):
    clip = VideoFileClip(rally_filename)
    s = 0
    sim_mat = list()
    while s < clip.duration - 3:
        ts, te = s, min(s + 6, clip.duration)
        sub_clip = clip.subclip(ts, te)
        os.system('rm -f /tmp/tmp.aac')
        sub_clip.audio.write_audiofile('/tmp/tmp.aac', codec='aac', logger=None)
        with torch.no_grad():
            audio_embed = model.get_audio_embedding_from_filelist(x = ['/tmp/tmp.aac'], use_tensor=True)
            sim_vec = list(torch.matmul(highlight_sample_emb, audio_embed.T).squeeze(1).numpy())
            sim_mat.extend(sim_vec)
            if max(sim_vec) >= 0.85:   
                clip.close()
                return max(sim_vec)
        s += 3
    clip.close()
    return max(sim_mat)

def main():
    highlight_emb = load_highlight_sample()
    target_rallys = 'rally_058_192_s_9_8.mp4 rally_113_457_s_15_10.mp4 rally_142_573_s_9_3.mp4 rally_210_910_s_20_18.mp4 rally_267_1092_s_0_0.mp4 rally_296_1211_s_19_10.mp4 rally_299_1228_s_20_12.mp4 rally_420_1821_s_16_15.mp4 rally_427_1848_s_19_19.mp4 rally_460_1996_s_3_4.mp4 rally_464_2009_s_7_4.mp4 rally_469_2021_s_9_7.mp4 rally_475_2043_s_12_10.mp4 rally_482_2067_s_17_12.mp4 rally_489_2087_s_20_16.mp4 rally_492_2096_s_20_19.mp4 rally_506_2133_s_4_3.mp4 rally_507_2136_s_5_3.mp4 rally_509_2141_s_6_4.mp4 rally_517_2160_s_10_8.mp4 rally_519_2172_s_12_8.mp4 rally_521_2179_s_14_8.mp4'.split(' ')
    for rally in rallys:
        break
        sim = highlight_judge(highlight_emb, 'rallys/' + rally)
        if sim < 0.85:   print('not passed', rally, sim)

    rallys = 'rally_001_66_s_0_0.mp4 rally_002_67_s_1_0.mp4 rally_003_70_s_2_0.mp4 rally_004_72_s_3_0.mp4 rally_005_74_s_4_0.mp4 rally_006_76_s_5_0.mp4 rally_007_78_s_5_1.mp4 rally_008_81_s_6_1.mp4 rally_009_82_s_7_1.mp4 rally_010_85_s_7_2.mp4 rally_011_86_s_8_2.mp4 rally_012_89_s_9_2.mp4 rally_013_90_s_10_2.mp4 rally_014_93_s_11_2.mp4 rally_015_102_s_12_2.mp4 rally_016_107_s_13_2.mp4 rally_017_109_s_14_2.mp4 rally_018_111_s_15_2.mp4 rally_019_114_s_15_3.mp4 rally_020_116_s_16_3.mp4'.split(' ')
    for rally in rallys:
        sim = highlight_judge(highlight_emb, 'rallys/' + rally)
        print('false test: rally', rally, sim)


