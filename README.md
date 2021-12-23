# Qtible
Qtible is the worlds best audio book player based for Linux on Qt.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

![Screenshot](images/screenshot.png?raw=true "Running under KDE with a dark theme")

# Features

## Open and Transparent
Qtible is open and transparent. Simply add your folders containing your
audio files to the software and listen to them. No closed format.
Whatever your MPV media player on Linux supports, qtible also supports!

Moreover, your listening progress and your bookmarks are saved as hidden
files among your normal audio files. So if you copy, move, share,
synchronize your files, your listening progress and bookmarks are also
there. You can use file sharing tools like Synchthing or NextCloud and
there you go: You just made your own cloud based audio book service!

## Save and Resume Progress
You can see how much you have listend through a book and then continue
just where you have left it.

## Sleep Timer
For those who use books to fall asleep (!) there's a sleep timer with
a gentle fadeout at the end of its time.

![Screenshot](images/screenshot-sleep-timer.png?raw=true "Sleep timer tab")

## Speed Adjustment
If the book/podcast is too boring or too fast you can simply adjust the
speed while keeping the natural pitch.

## Bookmarks
If you just heard something really amazing, don't forget to bookmark it.
You can also set a custom name for your bookmark to better remember it.

![Screenshot](images/screenshot-bookmarks.png?raw=true "Bookmarks tab")

## UNIX Friendly
Tired of heavy software packages written in scripting languages? Qtible
is based on native C++ language and Qt framework. Everything is tried to
be as close to UNIX philosophy as possible. When you add a folder to the
library, a symlink is created inside `$HOME/.config/qtible/library`.

Just like that!

When you are listening to an audio file, your progress is saved in a text
file named like `.qtible-progress.<name of your audio file>` inside the
same folder as your audio file.

Similarly, bookmarks are text files saved as hidden files with names like
`.qtible-bookmark.<some number>.<name of your audio file>`. That's why
you can simply copy, synchronize and move your files around along with
your listening progress and bookmarks. No black-box database format is
involved.

## Powered by MPV
Speaking of UNIX friendliness, the application is very minimalisting and
is made sure that it only does one thing and does it right! The media
play back is done by running an instance of **MPV**. Whatever MPV supports
on your computer, is also supported by Qtible.

# Donation
> Dear mate,

> Writing a software takes a long time. And if it is opensource, you
won't usually expect to earn some money out of it. If you liked it,
if it helped you, you may get back to the project website and make
some donation.

>Thank you.

# Installation
See INSTALL file for more info.

# dependencies
qt
mpv
libmpv
