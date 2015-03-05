use 5.006;
use strict;
use warnings FATAL => 'all';
use ExtUtils::MakeMaker;

our @EXCLUSIONS=();
our $MORE_MAKERULES='';

compile_DbcGrammar();

WriteMakefile(
	NAME             => 'Intel::IviPoc::AmbPluginGenerator',
	AUTHOR           => q{Intel IVI PoC <ivipoc@intel.com>},
	VERSION_FROM     => 'lib/Intel/IviPoc/AmbPluginGenerator.pm',
	ABSTRACT_FROM    => 'lib/Intel/IviPoc/AmbPluginGenerator.pm',
	LICENSE          => 'LGPL2.1',
	PL_FILES         => {},
	MIN_PERL_VERSION => 5.006,
	CONFIGURE_REQUIRES => {
		'ExtUtils::MakeMaker' => 0,
	},
	BUILD_REQUIRES => {
		'Test::More' => 0,
		'Parse::RecDescent' => 0,
	},
	PREREQ_PM => {
		'Exporter'=>0,
		'JSON'=>0,
		'Data::UUID'=>0,
		'File::Basename'=>0,
		'File::Spec'=>0,
		'Cwd'=>0,
		'Digest::SHA1'=>0,
		#'Config::General'=>0,
	},
	EXE_FILES => [ 'bin/dbc2json','bin/json2amb', 'bin/dbc2amb' ],
	dist  => { COMPRESS => 'gzip -9f', SUFFIX => 'gz', },
	clean => { FILES => 'AmbSignalMapper-*' },
);

# Overriding ExtUtils::MM methods
# --------------------------------------------------------------------
# Exclude the sources used to generate others from the build (See below).
sub MY::libscan {
my($self,$path)=@_;
	if(($path =~ /~$/) || grep($path eq $_, @EXCLUSIONS)){
		return ""; }
	$self->MY::SUPER::libscan($path); }

# Append any additional Makefile rules added by the following.
sub MY::postamble {
	shift->MY::SUPER::postamble(@_) . $MORE_MAKERULES; }

# Compile the RecDescent grammar during make; 
sub compile_DbcGrammar {
	push(@EXCLUSIONS,'blib/lib/Intel/IviPoc/DbcGrammar');
	push(@EXCLUSIONS,'blib/lib/Intel/IviPoc/MsgGrammar');
	$MORE_MAKERULES .= <<'MakeGrammar';

# Precompile the (Recursive Descent) DbcGrammar
pure_all :: $(INST_LIBDIR)/Intel/IviPoc/DbcGrammar.pm $(INST_LIBDIR)/Intel/IviPoc/MsgGrammar.pm

$(INST_LIBDIR)/Intel/IviPoc/DbcGrammar.pm: lib/Intel/IviPoc/DbcGrammar
	$(PERLRUN) -MParse::RecDescent - lib/Intel/IviPoc/DbcGrammar Intel::IviPoc::DbcGrammar
	$(NOECHO) $(MKPATH) $(INST_LIBDIR)/Intel/IviPoc
	$(MV) -f DbcGrammar.pm blib/lib/Intel/IviPoc/DbcGrammar.pm
	$(CHMOD) $(PERM_RWX) bin/dbc2json
	$(CHMOD) $(PERM_RWX) bin/json2amb

$(INST_LIBDIR)/Intel/IviPoc/MsgGrammar.pm: lib/Intel/IviPoc/MsgGrammar
	$(PERLRUN) -MParse::RecDescent - lib/Intel/IviPoc/MsgGrammar Intel::IviPoc::MsgGrammar
	$(NOECHO) $(MKPATH) $(INST_LIBDIR)/Intel/IviPoc
	$(MV) -f MsgGrammar.pm blib/lib/Intel/IviPoc/MsgGrammar.pm
	$(CHMOD) $(PERM_RWX) bin/dbc2amb

MakeGrammar
}


