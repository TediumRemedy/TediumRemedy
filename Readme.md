Tedium Remedy - is a chatting application that lets you meet strangers.
It's a frontend to Omegle, with a little prettier interface, no ads, and a few additional features, and is very practical to use.

TediumRemedy works on Windows, OS X, Linux.

Build using Qt Creator
Then after you've launched the app, the conversation with a stranger starts automatically.
You can start new conversation by just pressing Esc once.
You can switch mode - "Regular conversaion", "Chat with russian strangers", "Asking Questions (Spying)", "Answering Questions", by pressing 'Tab' (then the new mode kicks in after you press Esc). There's a mode indicator in the status bar.
In "Asking Questions" mode, before pressing Esc to start, make sure you've entered the question into typing box.

Currently app only supports Omegle.com (multilingual) and Chatvdvoem.ru (russian). In plans adding Moluren.net/chat (chinese chat) and other random strangers chats.

Controls:
* Tab - to switch modes (Regular chat, Russian chat, Answering questions, Asking questions)
* Control+"." - show preferences (you can enter your "likes" or select a language that your chat partners prefer to speak). 
This works only in "Regular" chat mode. The likes string should be in a format:
 `"first like" "second like" another`
If the interest consists of more than 1 word, it should be quouted with double quotation marks. Different interests can be separated by spaces or commas. 
If you enter likes, the chat will try to match you with strangers that have also some of these phrases-likes in their lists of likes. Otherwise, the chat will try to match you with strangers that prefer to speak selected language (which is English by default, but can be changed).
* Control+"z" - switch between night (dark) and day (bright) interface coloring
* Esc - find a new stranger and start a new chat (in "Question" mode you need to enter a question in typing box prior to pressing escape)

![Alt text](https://raw.github.com/TediumRemedy/TediumRemedy/master/screenshots/MainWindow.png)
![Alt text](https://raw.github.com/TediumRemedy/TediumRemedy/master/screenshots/Preferences.png)
