<?php

/* $Id: faq.php,v 1.3 2003/07/29 22:31:44 tim Exp $ */

$title="niemueller.de - palm software - Onager - FAQ";
$meta_desc="Palm GUI Frontend for the MLdonkey P2P network client";
$meta_keyw="Palm, PalmOS, donkey, edonkey, fasttrack, mldonkey, gnutella, file sharing, peer";
include("$DOCUMENT_ROOT/header.inc.php");
include("$DOCUMENT_ROOT/left.inc.php");

box_begin("Uni Donkey", "Last modified ".date ("F d Y H:i:s.", getlastmod()));
echo "<br/>";
print_heading_customimage("Onager", "Palm GUI Frontend for the MLdonkey P2P network client",
                          "onager_big.png", "index.php");
?>

<br/>
<a href="index.php">Back</a>

<h5>FAQ - Frequently asked questions</h5>
<b>Q:</b> Why does the code look so bad?<br/>
<b>A:</b> I started it, almost forgot about it and then just thought I had to finish it.
That's just the way it is.

<br/><br/>
<b>Q:</b> Can you compile a version that supports PalmOS version less than 3.5?<br/>
<b>A:</b> No, I can't. The 3.5 feature sets really made my life easier to this software done and I do
not plan to write more glue to make it run on older versions. Sorry.

<br/><br/>
<b>Q:</b> Where does the name "Onager" come from?<br/>
<b>A:</b> When I visited the <a href="http://www.zoo-koeln.de/" target="_new">Zoo Cologne</a>
          with my girl-friend in 2002 the first animal we saw was a donkey, this particular
          species was called "Onager"... Some more information may be found in the
          <a href="http://www.wikipedia.org/wiki/Onager" target="_new">Wikipedia</a>.

<br/><br/><br/>
<a href="index.php">Back</a>

<?php
box_end();

?>

<? include("$DOCUMENT_ROOT/right.inc.php"); ?>
<? include("$DOCUMENT_ROOT/footer.inc.php"); ?>
</body>
</html>
