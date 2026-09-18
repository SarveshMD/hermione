import mmap

with open("dummyfile", "rb") as f:
    with mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ) as mm:
        step = 4096
        total = 0
        for i in range(0, len(mm), step):
            total += mm[i]
        print("Done summing pages")
