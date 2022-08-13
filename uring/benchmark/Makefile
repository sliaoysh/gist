FLAMEGRAPH = libs/FlameGraph

.PHONY: build

build:
	clang++-12 -std=c++17 -O3 src/bench_syscall_write.cpp -o bench_syscall_write
	clang++-12 -std=c++17 -O3 src/bench_iouring_write.cpp -l:liburing.a -o bench_iouring_write

flamegraph:
	perf record -o iouring_write.perf -a -s -g ./bench_iouring_write output
	perf script -i iouring_write.perf | $(FLAMEGRAPH)/stackcollapse-perf.pl | grep bench_iouring_w > iouring_write.perf-folded
	$(FLAMEGRAPH)/flamegraph.pl --title "IO uring buffered write" iouring_write.perf-folded > iouring_write.svg
	perf record -o syscall_write.perf -a -s -g ./bench_syscall_write output
	perf script -i syscall_write.perf | $(FLAMEGRAPH)/stackcollapse-perf.pl | grep bench_syscall_w > syscall_write.perf-folded
	$(FLAMEGRAPH)/flamegraph.pl --title "Syscall buffered write" syscall_write.perf-folded > syscall_write.svg