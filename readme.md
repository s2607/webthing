
Webthing
====

A very basic unix web toolkit. 

In the end I would like to make something that lets you add the web to your workflow rather than a webbrowser where you do your work on the web. It should be oriented around open decentralized community pages rather than as a tool for coorperations to inject their software into your personal computer

Future feature ideas (in no order):
  1) form editing (including filling and submitting forms from command line arguments)
  2) persistent session information in either sqlite3 db or flat files
  3) some kind of built in search indexer would be amazing.
  4) xpath from the CLI would be cool... for that you could just use xmlstarlet and htmltidy though.
  5) support converting to from markdown for simple pages and editing from $EDITOR and PUTing the page back to the server would be neat if for nothing more than historical reasons
  6) redo interactive pager.
  7) Js (ducktape? it looks good but the scheduler cant handle thenables)
  8) generate postscript with embedded images etc.
  9) maybe a GUI with that atari game library.
  10) history (I *really* like how Tom Duff's Mothra browser does this)

##Warning

This is by no means complete. While I occasionally run valgrind's memcheck over it there are plenty of memory coruption bugs and loose ends. It's very much just an experament and shouldn't be seriously used.
