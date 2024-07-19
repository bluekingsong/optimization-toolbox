import requests
import json
import time
from datetime import datetime
import os

def get_file_list(url, post_content, post_data):
	post_data = { kv[0] : kv[1] for kv in  [x.split('=') for x in post_data.split(',')] }
	headers = {}
	for line in post_content.split('\n'):
		if line == '' or line.startswith('POST'):  continue
		kv = line.split(':')
		headers[kv[0]] = kv[1].strip()
	print('headers====\n', headers)
	print('data====\n', post_data)
	r = requests.post(url, headers=headers, data=post_data)
	print(r.status_code, r.reason)
	jresult = json.loads(r.text)
	hlsUrl = jresult['streams'][0]['hlsUrl']
	print(jresult)
	r = requests.get(hlsUrl)
	file_list = [ line for line in r.text.split('\n') if (not line.startswith('#')) and line != '' ]
	return file_list

def download_ts_files(video_host, request_content, file_list, output_path):
	ts_file_fmt = 'v.f721217'
	path = None
	headers = {}
	for line in request_content.split('\n'):
		if line == '':  continue
		if line.startswith('GET'):   
			path = line.split(' ')[1].split(ts_file_fmt)[0]
		else:
			kv = line.split(':')
			headers[kv[0]] = kv[1].strip()
	cnt = 0
	for file_name in file_list:
		target_filename = '%s/%s'%(output_path, file_name.split('?')[0])
		if os.path.exists(target_filename):  continue
		r = requests.get(video_host + path + file_name, headers=headers)
		if r.status_code == 200:
			with open(target_filename, 'wb') as fout:
				fout.write(r.content)
			time.sleep(1)
		print('download file_name=', file_name.split('?')[0], 'status_code=', r.status_code, datetime.now())

post_url = 'http://live.hbang.com.cn/api/fclive/play_video'
post_content = '''
POST /api/fclive/play_video HTTP/1.1
Host: live.hbang.com.cn
Accept: application/json, text/plain, */*
Accept-Encoding: gzip, deflate
Accept-Language: zh-CN,zh-Hans;q=0.9
Content-Type: application/x-www-form-urlencoded
Origin: http://live.hbang.com.cn
User-Agent: Mozilla/5.0 (iPhone; CPU iPhone OS 17_5_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148 MicroMessenger/8.0.43(0x18002b2f) NetType/WIFI Language/zh_CN
Connection: keep-alive
Referer: http://live.hbang.com.cn/www/?_v=422
Content-Length: 24
Cookie: Hm_lpvt_ef01d7b5a9da74545c443d32045bac52=1719205672; Hm_lvt_ef01d7b5a9da74545c443d32045bac52=1719193221; bonnypptk=tk_d4e4693fd763d43f1a47bae8653ad7b8d7799496
'''
post_data='''videoId=%s'''

video_id = 3603306963441061

video_host = 'http://1303027007.vod2.myqcloud.com'

request_content = '''GET /419f3a61vodtranscq1303027007/%s/v.f721217_0.ts?exper=&rlimit=3&sign=%s&t=%s&us=%s HTTP/1.1
Host: 1303027007.vod2.myqcloud.com
Accept-Language: zh-CN,zh-Hans;q=0.9
X-Playback-Session-Id: 6BCF16D4-F76C-4092-A59A-5CBBEDDFCABB
Accept: */*
User-Agent: Mozilla/5.0 (iPhone; CPU iPhone OS 17_5_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148 MicroMessenger/8.0.43(0x18002b2f) NetType/WIFI Language/zh_CN
Referer: http://live.hbang.com.cn/
Accept-Encoding: identity
Connection: Keep-Alive'''
server_path = '260451621253642698149042803'
sign = 'a9376b8b08c3c75938dd183142388af7'
t='667958df'
us='42378754'


output_path = 'bwf_world_tour_finals_2023'
video_id = 3448789369237061
server_path = '13c576ba3270835013821385460'
sign = 'c14d180facc4914551e57a1ee8791af3'
t='669a2569'
us='38065323'


file_names = get_file_list(post_url, post_content, post_data%(video_id))

download_ts_files(video_host, request_content%(server_path, sign, t, us), file_names, output_path)

