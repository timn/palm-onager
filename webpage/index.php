<?php

/* $Id: index.php,v 1.6 2003/07/29 23:20:44 tim Exp $ */

$title="niemueller.de - palm software - Onager";
$meta_desc="MLdonkey GUI for your PalmOS handheld.";
$meta_keyw="Palm, PalmOS, eDonkey, donkey, mldonkey, gui, frontend";
include("$DOCUMENT_ROOT/header.inc.php");
include("$DOCUMENT_ROOT/left.inc.php");

$version="0.1";
$basename="Onager";
$languages=array( "de" => "German/Deutsch",
                  "en" => "English",
                );

box_begin("Onager", "Last modified ".date ("F d Y H:i:s.", getlastmod()));
?>
<br/>

<table border="0" cellpadding="4">
 <tr>
  <td rowspan="2" valign="top"><img src="onager_big.png" border="0" alt="Onager"></td>
  <td><h3>Onager</h3>
 </tr>
 <tr>
  <td><h4>MLdonkey GUI for your PalmOS handheld.</h4></td>
 </tr>
</table>

<div class="maintext">
<h5>Introduction</h5>
This PalmOS applications acts as a really simple GUI to your
<a href="http://www.mldonkey.net" target="_new">mldonkey</a>. It uses
the low bandwidth (and low/almost not documented) GUI protocol to communicate
with your mldonkey. Right now it can display currently downloading and already
downloaded files and some basic stats. It can also commit files (I do not like
the auto commit feature...).<br/>

Some features:
<ul>
 <li>Show currently downloading and downloaded files (with chunk visualization)</li>
 <li>Commit downloaded files</li>
 <li>Basic status</li>
 <li>It's Open Source. You can hack it!</li>
</ul>

<br/>
Just to be sure that you heard: You will need a PalmOS device to run this
application. You can also use the Palm OS Emulator
<a href="http://www.palmos.com/dev/tools/emulator/" target="_new">POSE</a>.
Linux binaries can be found
<a href="http://sourceforge.net/project/showfiles.php?group_id=2189&release_id=84895" target="_new">here</a>.
<br/>
<i>Do not ask for ROM files</i>. I cannot and I do not want to share
my ROMs with you. Go to the mentioned POSE page, there is a description
on how to obtain ROM files from Palm Source directly and legally
without owning a Palm handheld.
<br/><br/>
Onager is available in multiple languages. Supported languages are right now (randomly ordered):
<ul>
 <li>German (Deutsch)</li>
 <li>English</li>
</ul>
If you want to help and translate Onager to another language grab the source and translate
the file resources/translation.en to your language. Read the file docs/TRANSLATION <i>before</i>
you start to translate the file as it contains note what you need to take care of
(basically some formatting guide lines, it's really simple and no programming
experience is needed). When you are done please send your new translation file via
<a href="mailto:tim@niemueller.de?Subject=Translate Onager">mail</a>.


<h5>FAQ - Frequently Asked Questions</h5>
There are some questions people are asking again and again. Read the common answers
<a href="faq.php">here</a>. <span style="color: red;">Please read this FAQ <u>before</u> sending
<u>any</u> questions!</span>


<h5>News</h5>
<b>2003/07/29</b><br/>
Finally there is some code that can be published. All the basic framework is in place
and it should be simple to add new features now. Read about the known issues below.

<h5>Thanks</h5>
Thanks for all the user contributions for those ideas!
<br/><br/>
A special thanks to my friend <a href="http://www.badcode.de" target="_new">Medha</a> who
pushed me to start this project and who helped to find some bugs.
All remaining bugs are my fault, of course. I probably inserted them after Medha's checks...


<h5>Screenshots</h5>
Some screenshots will give you an impression of the software.
<br/><br/>
<table border="0" width="100%" cellpadding="0">
 <tr>
  <td align="center" width="33%"><img src="screens/login.png" border="1" alt="Login"></td>
  <td align="center" width="34%"><img src="screens/downloads_list.png" border="1" alt="Downloads list"></td>
  <td align="center" width="33%"><img src="screens/downloads.png" border="1" alt="Downloads"></td>
 </tr>
 <tr>
  <th>Login</th>
  <th>Downloads list (red unavailable, bold currently downloading)</th>
  <th>One download</th>
 </tr>

 <tr>
  <td><br/>&nbsp;</td>
 </tr>

 <tr>
  <td align="center"><img src="screens/nofiles.png" border="1" alt="No files"></td>
  <td align="center"><img src="screens/stats.png" border="1" alt="Basic stats"></td>
  <td align="center"><img src="screens/downloaded.png" border="1" alt="Downloaded"></td>
 </tr>
 <tr>
  <th>No files</th>
  <th>Basic stats</th>
  <th>Downloaded files, can be commited from within the GUI</th>
 </tr>
</table>


<h5>Supported devices</h5>
Any PalmOS handheld device running PalmOS 3.5 or higher should work for Onager.
To get the real Onager feeling you need a handheld with a color display. See the
screenshots why... But it also supports 16 grays so you can use it on a grayscale device, too.
<br/><br/>I have tested Onager on:<br/>
Palm m515 (PalmOS 4.1, color, tested on real device, works perfectly)<br/>
Palm IIIc (PalmOS 3.5, color, tested on real device, works, see issues)<br/>
Palm IIIxe (PalmOS 4.1, gray, tested on real device, works, see issues)<br/>


<h5>How to get your Palm online</h5>
<table border="0">
 <tr><td>
  The easiest is to get a WLAN adapter for your Palm.
  I use the
  <a href="http://www.intel.com/support/network/xc/wireless/palm/index.htm" target="_new">Xircom WLAN Adapter</a>
  for Palm handhelds which has been discontinued and you can get it for about 100 Euro now.
  It is available for for Palm m5xx and m125 and works pretty good
  (Maybe in the future we will get that
  <a href="http://www.sychip.com/wlan-module.html" target="_new">SD WLAN adapter</a> :-)
If you have an older Palm you can use a null modem connection (your cradle or serial
HotSync cable is just right for this) to connect your Palm to the internet. This could
become handy if you mldonkey server is running the whole day but has no monitor. So
when you get up (late as always, university at 10:00 is just too early :-) you do not
need to boot your workstation to see whether your ISOs came through but you can just
have a look via your Palm... For an explanation on how to do this with a linux
machine have a look <a href="/wiki/?PalmNullModemPPP">here</a><br/>
But still the WLAN connection should be much more comfortable. If you have a Tungsten C
please report how it works! My setup in university is Palm m515, Xircom WLAN and
Mergic VPN.
 </td><td><img src="xircom_wireless.gif" alt="Xircom WLAN"></td>
 </tr>
</table>


<h5>Known issues</h5>
There are some bugs left in Onager that I did not fix yet (or don't even know of).
The most notably bug that I have encountered is that is can become very slow or
even unusable slow if you use a serial null modem PPP connection (see above on how
to get your Palm online). I could not yet figure out where this comes from and
since I have my WLAN adapter I do not need to fix it for myself. Anybody out there
wanna help?


<h5>Download</h5>
Onager is released and distributed under the terms of the
<a href="http://www.gnu.org/licenses/gpl.html#SEC1" target="_new">GNU General Public License (GPL)</a>.
<br/>
<span style="color: red;">By downloading and/or using the software you agree
to this license!</span>
<br/><br/>
You may download the Palm install file or sources provided as a .zip and as a .tar.gz file.
<br/>Latest <b>development</b> version is <span style="color:red;"><?=$version?></span> (no stable version yet).
<br/><br/>
<table border="0" cellpadding="0" cellspacing="0">
<? foreach ($languages as $key => $value) { ?>
<?   if (file_exists("{$basename}-{$version}_dist/{$basename}-{$version}-{$key}.tar.gz")) { ?>
 <tr>
  <td>Onager <?=$version?> (<?=$value?>)</td>
  <td>&nbsp; &nbsp;</td>
  <td><a href="<?=$basename?>-<?=$version?>_dist/<?=$basename?>-<?=$version?>-<?=$key?>.tar.gz">.tar.gz</a> (<?=filesize("{$basename}-{$version}_dist/{$basename}-{$version}-{$key}.tar.gz")?> Bytes)</td>
  <td>&nbsp;</td>
  <td><a href="<?=$basename?>-<?=$version?>_dist/<?=$basename?>-<?=$version?>-<?=$key?>.zip">.zip</a> (<?=filesize("{$basename}-{$version}_dist/{$basename}-{$version}-{$key}.zip")?> Bytes)</td>
 </tr>
<?   } ?>
<? } ?>
 <tr>
  <td>Download the source</td>
  <td>&nbsp; &nbsp;</td>
  <td><a href="<?=$basename?>-<?=$version?>_dist/<?=$basename?>-source-<?=$version?>.tar.gz">.tar.gz</a> (<?=filesize("{$basename}-{$version}_dist/{$basename}-source-{$version}.tar.gz")?> Bytes)</td>
  <td>&nbsp;</td>
  <td><a href="<?=$basename?>-<?=$version?>_dist/<?=$basename?>-source-<?=$version?>.zip">.zip</a> (<?=filesize("{$basename}-{$version}_dist/{$basename}-source-{$version}.zip")?> Bytes)</td>
 </tr>
</table>
<br/>

</div>

<?php
box_end();

?>

<? include("$DOCUMENT_ROOT/right.inc.php"); ?>
<? include("$DOCUMENT_ROOT/footer.inc.php"); ?>
</body>
</html>
