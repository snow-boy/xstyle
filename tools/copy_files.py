import sys
import shutil
import os


if __name__ == "__main__":
    dst_dir = sys.argv[-1]
    for i in range(1, len(sys.argv) - 1):
        if os.path.isdir(sys.argv[i]):
            dst_path = os.path.join(dst_dir, os.path.basename(sys.argv[i]))
            if os.path.exists(dst_path):
                shutil.rmtree(dst_path)
            shutil.copytree(sys.argv[i], dst_path)
        elif os.path.isfile(sys.argv[i]):
            shutil.copy(sys.argv[i], dst_dir)
    