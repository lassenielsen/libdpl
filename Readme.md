INTRODUCTION
------------
This is DPL (Dynamic Parser Library).
DPL is a library written for C++, to give a simple API for programming with
formal languages. Simplicity is obtained by working directly with BNFs
(Baccus-Nauer Form, also called Baccus Normal Form), and by not using a
precompilation step as most other parser frameworks.

This software is written by Lasse Nielsen (lasse.nielsen.dk@gmail.com),
and distributed under the GNU GPL v.3 or later.

This means that it is perfectly legal to obtain, use and maintain this software,
as long as the same rights are given for the derived products.

Information about the GNU GPL and its purpose can be found at the Free Software Foundation
(www.fsf.org) or more specifically at http://www.gnu.org/licenses/gpl.html

INSTALLATION
------------
This software requires the librcp library to be installed.

When the dependancies are installed, the library can be compiled and installed
with the following commands
 * `make config`
 * `make build`
 * `sudo make install`

There are example programs using the library in the examples folder.
Look at the example programs sorce code to see how the library is used.
The examplle programs are compiled by going to the examples folder and typing
 * make config
 * make build
