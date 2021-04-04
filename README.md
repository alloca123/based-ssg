based-ssg is a very based, minimal, and flexible static site generator written in pure C89 with no external deps.
# Installation
to install based-ssg, just run:
```
make
doas make install
```
# Usage
## template.html
You first need to write a template.html to use based-ssg.
This is pretty much just the template for all the html files based-ssg writes. 
A good example of this is in examples/template.html.

For titles, you should add
```
<title>INSERT-TITLE</title>
```
to your template.html

based-ssg finds INSERT-TITLE in your template.html, and replaces it with whatever title you put in files.conf
## files.conf
The config file is sorta similar to csv, but it is seperated by "^" and not ",".
The first option you need to set is markdown, then title, then html. Each option is seperated by "^"
check the conf file in the examples directory for some examples.
## actually running based-ssg
just run
```
based-ssg files.conf
```
and it will generate everything
