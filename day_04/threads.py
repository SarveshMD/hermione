import threading
import time

threading.Thread(target=lambda: time.sleep(300)).start()

time.sleep(300)
