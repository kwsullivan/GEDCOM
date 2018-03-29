ssh:
	ssh ksulli06@linux.socs.uoguelph.ca

sql:
	mysql -h dursley.socs.uoguelph.ca -u ksulli06 -p0947706

sqlshow:
	mysqlshow -h dursley.socs.uoguelph.ca -u ksulli06 -p0947706

remote:
	git remote add origin https://github.com/arkaydias/assign4.git

update: add commit push

add:
	git add Makefile package.json README.md parser app.js package-lock.json public uploads

commit:
	git commit -m "updating"

push:
	git push -f origin master