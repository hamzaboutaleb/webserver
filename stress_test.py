import threading
import http.client
import time
import sys

# --- Configuration ---
TARGET_HOST = "localhost"
TARGET_PORT = 8081
TARGET_PATH = "/"
NUM_THREADS = 500       # Number of concurrent "users"
REQUESTS_PER_THREAD = 50 # Total requests = NUM_THREADS * REQUESTS_PER_THREAD
# ---------------------

class StressTest:
    def __init__(self):
        self.total_requests = 0
        self.success_count = 0
        self.failure_count = 0
        self.latencies = []
        self.lock = threading.Lock()

    def make_request(self):
        start_time = time.time()
        try:
            conn = http.client.HTTPConnection(TARGET_HOST, TARGET_PORT, timeout=5)
            conn.request("GET", TARGET_PATH)
            response = conn.getresponse()
            
            # Read small part of body to ensure complete transfer
            response.read(1024) 
            
            latency = time.time() - start_time
            
            with self.lock:
                self.total_requests += 1
                if response.status == 200:
                    self.success_count += 1
                else:
                    self.failure_count += 1
                self.latencies.append(latency)
            
            conn.close()
        except Exception as e:
            with self.lock:
                self.total_requests += 1
                self.failure_count += 1
            # print(f"Request failed: {e}")

    def worker(self):
        for _ in range(REQUESTS_PER_THREAD):
            self.make_request()

    def run(self):
        print(f"--- Starting Stress Test ---")
        print(f"Target: http://{TARGET_HOST}:{TARGET_PORT}{TARGET_PATH}")
        print(f"Threads: {NUM_THREADS}, Requests per thread: {REQUESTS_PER_THREAD}")
        print(f"Total planned requests: {NUM_THREADS * REQUESTS_PER_THREAD}")
        
        start_test_time = time.time()
        threads = []
        for _ in range(NUM_THREADS):
            t = threading.Thread(target=self.worker)
            threads.append(t)
            t.start()

        for t in threads:
            t.join()
        
        end_test_time = time.time()
        total_time = end_test_time - start_test_time

        self.report(total_time)

    def report(self, total_time):
        avg_latency = sum(self.latencies) / len(self.latencies) if self.latencies else 0
        rps = self.total_requests / total_time if total_time > 0 else 0

        print(f"--- Results ---")
        print(f"Total Requests:      {self.total_requests}")
        print(f"Successful (200 OK): {self.success_count}")
        print(f"Failed:              {self.failure_count}")
        print(f"Total Time:          {total_time:.2f} seconds")
        print(f"Requests per Second: {rps:.2f}")
        print(f"Average Latency:     {avg_latency*1000:.2f} ms")
        if self.latencies:
            print(f"Min/Max Latency:     {min(self.latencies)*1000:.2f}ms / {max(self.latencies)*1000:.2f}ms")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        try:
            NUM_THREADS = int(sys.argv[1])
            REQUESTS_PER_THREAD = int(sys.argv[2])
        except:
            pass
            
    test = StressTest()
    test.run()
