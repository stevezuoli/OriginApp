#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
import os.path
import sys
import shutil
# 获取路径下所有文件和子目录名
def get_all_files(path):
    files = []
    os.path.walk(path, lambda x, y, z:files.extend([os.path.join(y, z0) for z0 in z]), None)
    files = map(lambda x: x[len(path):], files)
    return files

# 比较两个文件内容
def file_content_equal(path1, path2):
    if os.path.isdir(path1) and os.path.isdir(path2):
        return True
    with open(path1) as f1:
        with open(path2) as f2:
            if os.fstat(f1.fileno()).st_size != os.fstat(f2.fileno()).st_size:
                return False
            return f1.read() == f2.read()

if len(sys.argv) < 3:
    print "Usage: %s <Latester full packate dir> <previous full package dir1> [previous full package dir2] ..." % sys.argv[0]
    quit()

# 假定第一个参数为最新完整包目录，其后参数为要做增量升级的历次版本的完整包路径
paths = map(lambda x: x if x.endswith("/") else x + "/", sys.argv[1:])
all_files=map(get_all_files, paths)
common_files = reduce(lambda x, y: set(x) & set(y), all_files)
cur_path = paths[0]
same_files = []
for f in common_files:
    same = True
    # DuoKanInstall.sh强制为不更新
    if f.endswith("DuoKanInstall.sh"):
       same = False 
    for old_path in paths[1:]:
        if not file_content_equal(os.path.join(cur_path, f), os.path.join(old_path, f)):
            same = False
            break
    if same:
        same_files.append(f)

diff_files = set(all_files[0]) - set(same_files)
print diff_files

#增量升级目标文件夹
increase_path = cur_path[0:-1] + "_inc"
os.mkdir(increase_path)
for f in diff_files:
    src = os.path.join(cur_path, f)
    dst = os.path.join(increase_path, f)
    if os.path.isdir(src):
        if not os.path.exists(dst):
            os.makedirs(dst)
    else:
        path_name, base_name = os.path.split(dst)
        if not os.path.exists(path_name):
            os.makedirs(path_name)
        shutil.copy(src, dst)
