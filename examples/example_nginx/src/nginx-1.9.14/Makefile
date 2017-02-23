
default:	build

clean:
	rm -rf Makefile objs

build:
	$(MAKE) -f objs/Makefile

install:
	$(MAKE) -f objs/Makefile install

modules:
	$(MAKE) -f objs/Makefile modules

upgrade:
	/home/yaozhongkai/uter/examples/example_nginx/src/nginx-1.9.14_install/sbin/nginx -t

	kill -USR2 `cat /home/yaozhongkai/uter/examples/example_nginx/src/nginx-1.9.14_install/logs/nginx.pid`
	sleep 1
	test -f /home/yaozhongkai/uter/examples/example_nginx/src/nginx-1.9.14_install/logs/nginx.pid.oldbin

	kill -QUIT `cat /home/yaozhongkai/uter/examples/example_nginx/src/nginx-1.9.14_install/logs/nginx.pid.oldbin`
