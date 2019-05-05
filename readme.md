
Webthing
====

A very basic unix web toolkit. 

In the end I would like to make something that lets you add the web to your workflow rather than a webbrowser where you do your work on the web. It should be oriented around open decentralized community pages rather than as a tool for coorperations to inject their software into your personal computer

Form editing and submision works but searching duck duck go doesn't (because of a non standard redirect)

you can use the ? command to see a list of commands. The browser can be used in scripts by passing commands as arguments.  (so anything you can do interactively can be done from a script)

Future feature ideas (in no order):

  1) write a cgi proxy to handle caching, transformations, and javascrpt. This will let other tools interact with the session. (problems with this include: database syncronisation, and sharing the connection pool accross requests(processes) which we'll ignore for now.)
  2) javascript would be cool.
  3) fix non standard redirects.
  4) css


##Warning

This is by no means complete. While I occasionally run valgrind's memcheck over it there are plenty of memory coruption bugs and loose ends. It's very much just an experament and shouldn't be seriously used.
