[Mongrel2](http://mongrel2.org/home)
=============

This mirror is little buggy, but already usable.

Mongrel2 is the complete redesign of the Mongrel web server to be an application, language, and network architecture agnostic web server. It's written by the original author of Mongrel, and uses the Mongrel HTTP parser, but everything else is new. The goal is to create a web server specifically designed for web applications over serving files.

Here's the planned features for Mongrel2:
* Programming language agnostic design that lets you use whatever language you want, even to configure and manage it.
* Designed to handle HTTP, Flash XMLSockets, or WebSockets (coming soon) on the same socket transparently, and then route messages from those to ZeroMQ, HTTP, or raw socket backends
* Break the strict request==response mapping and let you translate one request or message into multiple backends to produce a more complex response. This will let you create scalable architectures using any backend language you want.
* Uses a MVC style configuration design, with sqlite3 as the "Model" and Python scripts as the "View". There's a m2sh program that lets you manage the server, it's configuration, and logs about configuration activity.
* Use the proven and rock solid Mongrel HTTP Parser that many other web application servers use. There have been very few security flaws in this parser, and it actively blocks many attacks due to its design.
* Use the latest event systems (poll, epoll, kqueue) but rather than force you to understand async events, it will use a fast context switching coroutine system based on ucontext or assembler.
* Support all of the usual web server features, but it will defer to being designed specifically for applications over high-speed file serving. Since Mongrel2 will be written in C it should do files pretty well too, but easy application architectures and operations support will come first.

Check out the [GettingStarted](http://mongrel2.org/wiki?name=GettingStarted) instructions if you want to play with it right now.

### Status ###
* Completely self-hosting with functioning demonstrations for everything.
* Configurable using a Python based configuration system and SQLite3.
* Complete HTTP handling and correct proxying, including 0MQ handling of HTTP requests or JSSockets.
* Uses poll for both 0MQ and file/socket I/O, with others on the way.
* Handles Flash Socket protocol with routing to 0MQ PUB/SUB backends.
* Released sample for testing the chat demo in Python, http handlers, and other examples.
* Handler libraries in PHP, Python, C++, and Ruby.

Currently being worked on features:
* Look at the [Tickets](http://www.mongrel2.com/reportlist) to see.

### License ###
Mongrel2 is BSD Licensed using the 3 clause BSD license, which basically means you can use it how you want but you can't promote your business activities using the names Mongrel2 Project, or its contributors.

You of course can tell anyone you want that that you're using Mongrel2, that your services or your company are based on Mongrel2. You just can't say that our project endorses your business without explicit permission.

You can read the full [License](http://mongrel2.org/wiki?name=License) here.
