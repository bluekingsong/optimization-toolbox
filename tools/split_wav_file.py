import os
from pydub import AudioSegment
from datetime import datetime

def cut_clip(root_path, wav_filename, output_path='/tmp'):
    clip = AudioSegment.from_wav(os.path.join(root_path, wav_filename))
    for s in range(0, int(clip.duration_seconds), 3):
        e = min(int(clip.duration_seconds), s + 5)
        sub_clip = clip[s*1000 : e*1000]
        output_audio_file = output_path + '/' + wav_filename.split('/')[-1].replace('.wav', '') + '_t%d_%d.wav'%(s, e)
        if not os.path.exists(output_audio_file):
            sub_clip.export(output_audio_file, format="wav")
root_path = 'wav_files'
line_cnt = 0
for line in open(os.path.join(root_path, 'train.tsv')):
    line_cnt += 1
    if line_cnt == 1:  continue
    filename = line.split(' ')[0]
    cut_clip(root_path, filename, 'five_seconds_wav_files')
    print(filename, 'processed', datetime.now())
