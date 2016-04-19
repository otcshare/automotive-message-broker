#!perl -T
use 5.006;
use strict;
use warnings FATAL => 'all';
use Test::More;

plan tests => 1;

BEGIN {
    use_ok( 'Intel::IviPoc::AmbPluginGenerator' ) || print "Bail out!\n";
}

diag( "Testing Intel::IviPoc::AmbPluginGenerator $Intel::IviPoc::AmbPluginGenerator::VERSION, Perl $], $^X" );
