#!/usr/bin/python

#find m from n
def FindMFromN(m, n):
	res_bufs = []
	if m > n:
		return res_bufs
	work_buf = []	
	index_range = range(n)
	for i in index_range:
		work_buf.append(0)
	for i in range(m):
		work_buf[i] = 1
	while 1:
		cur_res_buf = []
		for i in range(n):
			if work_buf[i] == 1:
				cur_res_buf.append(i)
		res_bufs.append(cur_res_buf)
		reverse_index = -1
		former_val = 0
		for i in index_range:
			if work_buf[i] == 1:
				former_val = 1
			elif former_val == 1:
				work_buf[i] = 1
				work_buf[i-1] = 0
				reverse_index = i-1
				break
		if -1 != reverse_index:
			zero_num = 0
			if reverse_index > 1:
				for i in range(reverse_index):
					if work_buf[i] == 0:
						zero_num += 1
				one_index = reverse_index - zero_num
				if zero_num != 0:
					for i in range(reverse_index):
						if i < one_index:
							work_buf[i] = 1
						else:
							work_buf[i] = 0
		else:
			break
	return res_bufs		



			

