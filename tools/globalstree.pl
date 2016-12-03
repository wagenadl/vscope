#!/usr/bin/perl -w

my $icol=0;
my @colors = (
  "Blue",
  "Brown",
  "Burlywood",
  "Chocolate",
  "Coral",
  "Crimson",
  "DodgerBlue",
  "ForestGreen",
  "HotPink",
  "IndianRed",
  "Indigo",
  "Magenta",
  "MidnightBlue",
  "Navy",
  "Olive",
  "OliveDrab",
  "Orange",
  "OrangeRed",
  "Orchid",
  "PapayaWhip",
  "PeachPuff",
  "Peru",
  "Pink",
  "Plum",
  "PowderBlue",
  "Red",
  "RosyBrown",
  "RoyalBlue",
  "SaddleBrown",
  "Salmon",
  "SandyBrown",
  "SeaGreen",
  "Seashell",
  "Sienna",
  "Silver",
  "SkyBlue",
  "SlateBlue",
  "SlateGray",
  "Snow",
  "SpringGreen",
  "SteelBlue",
  "Tan",
  "Teal",
  "Thistle",
  "Tomato",
  "Turquoise",
  "Violet",
  "Wheat",
  "White",
  "WhiteSmoke",
  "Yellow",
  "YellowGreen",
);

my %var2cls;
my %lbls;
my %clscount;
open GLOBALS, "<toplevel/globals.h";
while (<GLOBALS>) {
  if (/static class (.*?) \*(.*);/) {
    my $c = $1;
    my @k = split(/, \*/,$2);
    for (@k) {
      $var2cls{$_} = $c;
      $clscount{$c}++;
    }
  }
}
close GLOBALS;
for my $c (keys %clscount) {
  my $n = $clscount{$c};
  $lbls{$c} = "$c ($n)" if $n>1;
}

open OUT, ">/tmp/gtree.dot";
print OUT <<"EOF";
digraph "Globals" {
    rankdir=LR;
    overlap=scale;
    size="20,10";
    ratio="fill";
    fontsize="10";
    fontname="Sans";
	clusterrank="local";
EOF

open GREP, "grep Globals:: */*cpp|";
my %file2class;
my %done;
my %frmcol;
while (<GREP>) {
  chomp;
  my ($k,$v) = split(/:/,$_,2);
  if (!exists($file2class{$k})) {
    my $h = $k; $h =~ s/cpp$/h/;
    $file2class{$k} = $k;
    if (open HEADER, "<$h") {
      while (<HEADER>) {
	if (/^class ([^ :]*)/) {
	  $file2class{$k} = $1;
	  last;
	}
      }
      close HEADER;
    } else {
      print STDERR "Cannot open $h\n";
    }
  }
  my $from = $file2class{$k};
  $from = $lbls{$from} if exists($lbls{$from});
  
  $v =~ /Globals::([a-zA-Z_0-9]+)/ or next;
  my $to = exists($var2cls{$1}) ? $var2cls{$1} : $1;
  $to = $lbls{$to} if exists($lbls{$to});
  unless (exists($frmcol{$from})) {
    my $a = .3+rand()*.7;
    my $b = .3+rand()*.7;
    my $c = .3+rand()*.7;
    my $col=$colors[$icol]; $icol++; #"$a $b $c";
    print OUT "\"$from\" [color=\"$col\", fontcolor=\"$col\"];\n";
    $frmcol{$from} = $col;
  }
  print OUT "       \"$from\" -> \"$to\" [color=\"$frmcol{$from}\"];\n"
    unless exists($done{"$from:$to"}) || $to =~ /^.$/;
  $done{"$from:$to"} = 1;
}
close GREP;
print OUT "}\n";
close OUT;

system("dot /tmp/gtree.dot -Tpng -o/tmp/gtree.png");
