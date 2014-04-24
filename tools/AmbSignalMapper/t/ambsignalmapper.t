#!perl -T
use 5.006;
use strict;
use warnings FATAL => 'all';
use Test::More;
use Intel::IviPoc::AmbPluginGenerator;

plan tests => 1;

# TEST
{
	my $src_text = "This is a text strig to check if AmbTmpl, AMBTMPL and ambtmpl are replaced with myplugin respectively and with proper caseing";
	my $replace_text = "MyPlugin";
	my $dst_text = "This is a text strig to check if MyPlugin, MYPLUGIN and myplugin are replaced with myplugin respectively and with proper caseing";
	my $ret = Intel::IviPoc::AmbPluginGenerator::replaceTemplateStrings($src_text, $replace_text);
	ok ( $ret eq $dst_text, "  replaceTemplateStrings works" );
}

