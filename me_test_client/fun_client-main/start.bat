docker build . -t cppenv
docker rm -f cppenv
docker run --net=host --cap-add sys_ptrace -d --name cppenv --user root --memory=4g -v %cd%\..:/home/user/work cppenv tail -F /dev/null
