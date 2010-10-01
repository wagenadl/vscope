#!/usr/bin/perl -w

use strict;
use XML::LibXML;

my $infile = "gui/config.xml";
$infile = shift @ARGV if @ARGV;
print "Reading from $infile\n";

my $parser = XML::LibXML->new();
my $doc = $parser->parse_file($infile);
my $root = $doc->documentElement();

parse($doc,0);

sub parse {
  my $elt = shift;
  my $level = shift;
  print "  " x $level, $elt->nodeName;
  if ($elt->hasChildNodes()) {
    print ":\n";
    for (my $n=$elt->firstChild; $n; $n=$n->nextSibling()) {
      parse($n,$level+1);
    }
  } else {
    print "\n";
  }
}


