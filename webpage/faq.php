<?php

/* $Id: faq.php,v 1.1.1.1 2003/07/09 12:05:16 tim Exp $ */

$title="niemueller.de - palm software - UniDonkey - FAQ";
$meta_desc="Palm GUI Frontend for the MLdonkey P2P network client";
$meta_keyw="Palm, PalmOS, donkey, edonkey, fasttrack, mldonkey, gnutella, file sharing, peer";
include("$DOCUMENT_ROOT/header.inc.php");
include("$DOCUMENT_ROOT/left.inc.php");

box_begin("Uni Donkey", "Last modified ".date ("F d Y H:i:s.", getlastmod()));
echo "<br/>";
print_heading_customimage("Uni Donkey", "Palm GUI Frontend for the MLdonkey P2P network client",
                          "unidonkey_big.png", "index.php");
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

<br/><br/><br/>
<a href="index.php">Back</a>

<?php
box_end();

?>

<? include("$DOCUMENT_ROOT/right.inc.php"); ?>
<? include("$DOCUMENT_ROOT/footer.inc.php"); ?>
</body>
</html>
