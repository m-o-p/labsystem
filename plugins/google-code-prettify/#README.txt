If you want to use the javascript code formatter google-code-prettify, do the following:

- Download the code from http://code.google.com/p/google-code-prettify/
  e.g. wget [Link you get when clicking on download on the page]
- Extract the archive to plugins/google-code-prettify/
  Make sure you do not have nested subfolders, so the scrips must be in THIS directory and not in another sub directory.
  e.g. tar -jxvf prettify-small-1-Jun-2011.tar.bz2
- Edit the ini/configBase/defaultSystemLayout.ini
  Set google-code-prettify    = "TRUE"
  
  To use it:
  
  <pre class="prettyprint"><code class="language-java">...</code></pre>
  
<pre class="prettyprint linenums:4">// This is line 4.
foo();
bar();
baz();
boo();
far();
faz();
<pre>