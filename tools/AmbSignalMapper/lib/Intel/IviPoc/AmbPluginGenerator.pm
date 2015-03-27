#Copyright (C) 2014  Intel Corporation
#
#This library is free software; you can redistribute it and/or
#modify it under the terms of the GNU Lesser General Public
#License as published by the Free Software Foundation; either
#version 2.1 of the License, or (at your option) any later version.
#
#This library is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#Lesser General Public License for more details.
#
#You should have received a copy of the GNU Lesser General Public
#License along with this library; if not, write to the Free Software
#Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

package Intel::IviPoc::AmbPluginGenerator;
our @ISA = qw(Exporter);
our @EXPORT = qw(processPlugin);

use 5.006;
use strict;
use warnings FATAL => 'all';
use Exporter;

use JSON;
use Data::UUID;
use File::Spec;
use Digest::SHA qw(sha1_hex);
use Intel::IviPoc::AmbCommon  qw(readFileContent);

=head1 NAME

Intel::IviPoc::AmbPluginGenerator - The great new Intel::IviPoc::AmbPluginGenerator!

=head1 VERSION

Version 0.01

=cut

our $VERSION = '0.01';


=head1 SYNOPSIS

Module provides subroutines for reading input JSON file and generating Automotive Message Broker
plugin project.

The following little code snippet shows the module usage.

	use Intel::IviPoc::AmbPluginGenerator qw(processPlugin);

	my $hashingAllowed = "E";
	my $inputfile = "myfile.json";
	my $targetDir = '/home/user/project/automotive-message-broker/plugins';
	processPlugin ( $hashingAllowed, $inputfile, $targetDir );
	...

=head1 EXPORT

The following convenience methods are provided by this module. They are
exported by default:

=over 4

=item C<processPlugin $json_filename>

For given json file name generates the Automotive Message Broker plugin

=back

=head1 SUBROUTINES/METHODS

=head2 processPlugin

Copies all template files into target folder while replacing the name
with newly generated plugin name. Then generates C++ and IDL definitions
based on information comming from input JSON file.

=cut

my $hashingAllowed = 'E';       # Enabled by default

sub processPlugin {
	$hashingAllowed = $_[0];
	my $jsonfile = $_[1];
	my $targetDir = $_[2];

	# Load the json
	my $json_text = &readFileContent( $jsonfile );

	my $json = JSON->new;
	$json = $json->utf8;

	my $dbcjson = $json->decode( $json_text );
	if ($hashingAllowed eq 'E' ) {
		&encryptAmbPropertyNames( $dbcjson );
	}

	my $pluginName = $dbcjson->{'pluginName'};

	my $templatesDir = 'templates/';
	my $pluginDir = File::Spec->catdir( ($targetDir, lc($pluginName). '_plugin/') );

	# make new folder
	&createDirectory( $pluginDir );

	my @templatesFiles = ( "CMakeLists.txt"
						 , "ambtmpl_cansignal.h"
						 , "ambtmpl_cansignal.cpp"
						 , "ambtmpl_plugin.h"
						 , "ambtmpl_plugin.cpp"
						 , "ambtmpl_cansignals.h"
						 , "ambtmpl_plugin.idl"
						 );

	my @pluginFiles = ( "CMakeLists.txt"
					  , lc ($pluginName) . "_cansignal.h"
					  , lc ($pluginName) . "_cansignal.cpp"
					  , lc ($pluginName) . "_plugin.h"
					  , lc ($pluginName) . "_plugin.cpp"
					  , lc ($pluginName) . "_cansignals.h"
					  , lc ($pluginName) . "_plugin.idl"
					  );

	my @generationSubs = ( undef
						 , undef
						 , undef
						 , \&generateUuid
						 , \&generateCppImplTypes
						 , \&generateSignalsTypes
						 , \&generateIdlTypes
						 );

	my $templateFile = '';
	my $pluginFile = '';
	my ($volume, $directory) = File::Spec->splitpath( $INC{'Intel/IviPoc/AmbPluginGenerator.pm'} );
	for my $i (0..scalar(@pluginFiles)-1) {
		# First join templates folder and filename
		$templateFile = File::Spec->catfile( ($templatesDir), $templatesFiles[$i] );
		# Now prepend the module full path
		$templateFile = File::Spec->catpath( $volume, $directory, $templateFile );
		# Join target directory with target filename
		$pluginFile = File::Spec->catfile( ($pluginDir), $pluginFiles[$i] );
		# Generate each plugin files
		&generatePluginFile( $templateFile, $pluginFile, $dbcjson, $generationSubs[$i]);
	}
}

=head2 generatePluginFile

Copies one template file into target folder while replacing the name
with newly generated plugin name. Then generates definitions
based on information comming from input JSON file.

=cut

sub generatePluginFile {
	my $srcFileName = $_[0];
	my $dstFileName = $_[1];
	my $dbcjson = $_[2];
	my $generationSub = $_[3];

	my $pluginName = $dbcjson->{'pluginName'};

	# Open template file
	my $content = &readFileContent( $srcFileName );
	$content = &replaceTemplateStrings( $content, $pluginName );

	if (defined $generationSub) {
		my $generatedCode = $generationSub->( $dbcjson );
		my $place = '\/\*GENERATED_CODE\*\/';
		$content =~ s/$place/$generatedCode/g;
	}

	# Create new file
	my $pluginFileHandle = &createFile( $dstFileName );

	# Copy data from one file to another.
	print $pluginFileHandle $content;

	# close the file
	close ($pluginFileHandle);
}

=head2 createDirectory

Creates directory for plugin.

=cut

sub createDirectory {
	my $dirName = $_[0];

	unless(-e $dirName or mkdir $dirName) {
		die "Unable to create directory '$dirName' $!";
	}
}

=head2 createFile

Creates file and returns a file handle to it.

=cut

sub createFile {
	my $fileName = $_[0];

	# Open file or die
	open(my $fileHandle, '>', $fileName)
		or die "Could not open file '$fileName' $!";

	return $fileHandle;
}

=head2 replaceTemplateStrings

Replaces all occurencies of template specific symbols with plugin name.

=cut

sub replaceTemplateStrings {
	my $text = $_[0];
	my $pluginName = $_[1];

	$text =~ s/AmbTmpl/$pluginName/g;
	$pluginName = lc ($pluginName);
	$text =~ s/ambtmpl/$pluginName/g;
	$pluginName = uc ($pluginName);
	$text =~ s/AMBTMPL/$pluginName/g;
	return $text;
}

=head2 generateUuid

For all signals generates C++ property instantiation.
Returns C++ code to be placed into the target plugin.

=cut

sub generateUuid {
	my $dbcjson = $_[0];

	my $ug = new Data::UUID;
	my $uuidText = $ug->create_str();

	return $uuidText;
}

=head2 generateCppImplTypes

For all signals generates C++ property instantiation.
Returns C++ code to be placed into the target plugin.

=cut

sub generateCppImplTypes {
	my $dbcjson = $_[0];

	my $registerMessageText = '';
	my $hexValue = ();

	my @engineControlUnits = @{$dbcjson->{'electronicControlUnits'}};
	for my $ecui (0..scalar(@engineControlUnits)-1) {
		if ( exists( $engineControlUnits[$ecui]{'messages'} ) ) {
			my @messages = @{$engineControlUnits[$ecui]{'messages'}};
			for my $msgi (0..scalar(@messages)-1) {
				$hexValue = '0x' . uc ( sprintf( "%x", $messages[$msgi]{'canId'} ) );
				$registerMessageText .= "    registerMessage($hexValue, $messages[$msgi]{'canDlc'}";

				my @signals = @{$messages[$msgi]{'signals'}};
				foreach my $signal ( @signals ) {
					my $type = $signal->{'AMBPropertyType'};
					$registerMessageText .= &generateCppProperty( $signal, $type);
				}

				$registerMessageText .= "\n                   );\n";
			}
		}
	}

	return $registerMessageText;
}

=head2 generateCppProperty

For given signal generates ADDPROPERTY calls and returns C++ property
definitions.

=cut

sub generateCppProperty {
	my $signal = $_[0];
	my $type = $_[1];

	my $generatedText = '';
	my $zonesInUse = 0; # TODO this needs to be in config file

	if ( exists( $signal->{'AMBPropertyName'} ) ) {
		my $ambPropertyName = $signal->{'AMBPropertyName'};

		# TODO CANSignal needs to take zone as argument
		#my $zone = 'Zone::None';
		#if ($zonesInUse) {
		#    $zone = &calculateZone( $ambPropertyName );
		#}
		$generatedText .= "\n                   , new ${ambPropertyName}Type()";
	}
	return $generatedText;
}

=head2 generateSignalsTypes

For all signals generates C++ property definitions.
Returns C++ code to be placed into the target plugin.

=cut

sub generateSignalsTypes {
	my $dbcjson = $_[0];

	my $enumsText = '';
	my $propertiesText = '';

	# First generate the c++ enums from signals with values
	my @engineControlUnits = @{$dbcjson->{'electronicControlUnits'}};
	for my $ecui (0..scalar(@engineControlUnits)-1) {
		if ( exists( $engineControlUnits[$ecui]{'messages'} ) ) {
			my @messages = @{$engineControlUnits[$ecui]{'messages'}};
			for my $msgi (0..scalar(@messages)-1) {
				my @signals = @{$messages[$msgi]{'signals'}};
				foreach my $signal ( @signals ) {
					my $type = $signal->{'AMBPropertyType'};
					$enumsText .= &generateEnumOrValues( $signal, $type);
					$propertiesText .= &generatePropertyClasses( $signal, $type );
				}
			}
		}
	}
	return $enumsText . $propertiesText;
}

=head2 generateEnumOrValues

For given signal generates enums or c++ constants and returns C++ types
definitions.

=cut

sub generateEnumOrValues {
	my $signal = $_[0];
	my $type = $_[1];

	my $generatedText = "";

	my $ambPropertyName = $signal->{'canId'};
	if ( exists( $signal->{'AMBPropertyName'} ) ) {
		$ambPropertyName = $signal->{'AMBPropertyName'};
	}

	if ( exists( $signal->{'values'} ) ) {
		my @dupvalues = @{$signal->{'values'}};
		my @values = sort { $$a{'value'} <=> $$b{'value'} } (&removeDuplicates (\@dupvalues));
		my $hexValue = ();

		if ( $type eq 'enum' or $type =~ m/int/) {
			# Start with comments
			$generatedText .= "/**< $ambPropertyName\n";
			for my $vali (0..scalar(@values) -1 ) {
				$hexValue = '0x' . uc ( sprintf( "%x", $values[$vali]->{'value'} ) );
				$generatedText .= " * $hexValue = $values[$vali]->{'description'}\n";
			}
			$generatedText .= " */\n";
		}

		if ( $type eq 'enum' ) {
			# Enum definition
			$generatedText .= "namespace ${ambPropertyName}s {\n";
			$generatedText .= "enum ${ambPropertyName}Type {\n";

			# Generate enum values
			for my $vali (0..scalar(@values) -1 ) {
				$hexValue = '0x' . uc ( sprintf( "%x", $values[$vali]->{'value'} ) );
				$generatedText .= "    $values[$vali]->{'name'} = $hexValue";
				if ($vali != scalar(@values)-1 ) {
					$generatedText .= ",";
				}
				$generatedText .= "\n";
			}
			$generatedText .= "};\n";
			$generatedText .= "}\n\n";
		} elsif ( $type =~ m/int/ ) {
			$generatedText .= "namespace ${ambPropertyName}s {\n";
			# Generate values
			for my $vali (0..scalar(@values) -1 ) {
				$hexValue = '0x' . uc ( sprintf( "%x", $values[$vali]->{'value'} ) );
				$generatedText .= "static const $type $values[$vali]->{'name'} = $hexValue;";
				$generatedText .= "\n";
			}
		$generatedText .= "}\n\n";
		}
	}

	return $generatedText;
}

=head2 generatePropertyClasses

For given signal generates documentation, VehicleProperty::Property
properties with values of stringified properties names.
lastly using CANSIGNAL macro generates C++ classes
for given signal.

Returns C++ definitions of one signal property.

=cut

sub generatePropertyClasses {
	my $signal = $_[0];
	my $type = $_[1];

	my $generatedText = '';

	my $ambPropertyName = $signal->{'canId'};
	if ( exists( $signal->{'AMBPropertyName'} ) ) {
	$ambPropertyName = $signal->{'AMBPropertyName'};
	}

	my $byteOrdering = "Endian::Intel";                 # LittleEndian by default
	if ( exists( $signal->{'byteOrdering'} ) and $signal->{'byteOrdering'} eq '0') {
	$byteOrdering = "Endian::Motorola";                 # BigEndian
	}

	my $signedness;
	if ($signal->{'signedness'} eq '+') {
	$signedness = "Signedness::Unsigned";               # Unsigned
	} else {
	$signedness = "Signedness::Signed";                 # Signed
	}

	my $convertFromFunction = "nullptr";
	if ( exists( $signal->{'AMBConversionFrom'} ) ) {
		$convertFromFunction = $signal->{'AMBConversionFrom'};
	}

	my $convertToFunction = "nullptr";
	if ( exists( $signal->{'AMBConversionTo'} ) ) {
		$convertToFunction = $signal->{'AMBConversionTo'};
	}

	$generatedText .= "\n";
	$generatedText .= "/**< $ambPropertyName.\n";

	my $typeBasedText = '';
	my $cppType;

	if ( $type =~ m/enum/ ) {
		if ( exists( $signal->{'values'} ) ) {
			$generatedText .= " *\@see ${ambPropertyName}s::${ambPropertyName}Type\n";
			$cppType = "${ambPropertyName}s::${ambPropertyName}Type";
		}
	} elsif ( $type =~ m/bool/ ) {
		if ( exists( $signal->{'values'} ) ) {
			my @dupvalues = @{$signal->{'values'}};
			my @values = sort { $$a{'value'} <=> $$b{'value'} } (&removeDuplicates (\@dupvalues));
			my $hexValue = ();
			for my $vali (0..scalar(@values) -1 ) {
				$hexValue = '0x' . uc ( sprintf( "%x", $values[$vali]->{'value'} ) );
				$generatedText .= " * $hexValue = $values[$vali]->{'description'}\n";
			}
		}
		$cppType = "$type";
	} elsif ( $type =~ m/int8/ ) {
		$cppType = "char";
	} elsif ( $type =~ m/int32/ ) {
		if ( $type eq 'uint32_t' ) {
			$cppType = "$type";
		} else {
			$cppType = "int";
		}
	} else {    # (u)int16, (u)int64
		$cppType = "$type";
	}

	 $typeBasedText .= "CANSIGNAL($ambPropertyName, $cppType, $signal->{'startBit'}, $signal->{'length'}, $byteOrdering, $signedness, $signal->{'factor'}, $signal->{'offset'}, static_cast<$cppType>($signal->{'minValue'}), static_cast<$cppType>($signal->{'maxValue'}), $convertFromFunction, $convertToFunction)\n";

	$generatedText .= " */\n";
	my $shownPropertyName = $ambPropertyName;
	if ($hashingAllowed eq 'E' ) {
		$shownPropertyName = $signal->{'AMBPropertyNameEnc'};
	}
	$generatedText .= "const VehicleProperty::Property $ambPropertyName = \"$shownPropertyName\";\n";
	$generatedText .= $typeBasedText;

	return $generatedText;
}

=head2 generateIdlTypes

For all signals generates WebIDL property documentation.
Returns IDL code to be placed into the target plugin.

=cut

sub generateIdlTypes {
	my $dbcjson = $_[0];

	my $generatedText = '';

	my @engineControlUnits = @{$dbcjson->{'electronicControlUnits'}};
	for my $ecui (0..scalar(@engineControlUnits)-1) {
		if ( exists( $engineControlUnits[$ecui]{'messages'} ) ) {
			my @messages = @{$engineControlUnits[$ecui]{'messages'}};
			for my $msgi (0..scalar(@messages)-1) {
				my @signals = @{$messages[$msgi]{'signals'}};
				foreach my $signal ( @signals ) {
					my $type = $signal->{'AMBPropertyType'};
					$generatedText .= &generateIdlProperty( $signal, $type);
				}
			}
		}
	}
	return $generatedText;
}

=head2 generateIdlProperty

For given signal generates WebIDL documentation and returns WebIDL
definitions of one signal property.

=cut

sub generateIdlProperty {
	my $signal = $_[0];
	my $type = $_[1];
	my $generatedText = '';

	my $ambPropertyName = $signal->{'canId'};
	if ( exists( $signal->{'AMBPropertyName'} ) ) {
		$ambPropertyName = $signal->{'AMBPropertyName'};
	}

	$generatedText .= "[NoInterfaceObject]\n";
	$generatedText .= "interface org.automotive.${ambPropertyName} : VehiclePropertyType {\n";
	if ( $type eq 'enum' ) {
		if ( exists( $signal->{'values'} ) ) {
			my @dupvalues = @{$signal->{'values'}};
			my @values = sort { $$a{'value'} <=> $$b{'value'} } (&removeDuplicates (\@dupvalues));

			my $hexValue = ();
			for my $vali (0..scalar(@values) -1 ) {
				# TODO const unsigned short migth be not enough, guess type based on values
				$hexValue = '0x' . uc (sprintf( "%x", $values[$vali]->{'value'} ) );
				$generatedText .= "    const unsigned short " . uc($values[$vali]->{'name'}) . " = $hexValue;\n";
			}
		}
	}

	$generatedText .= "\n";
	$generatedText .= "    /**  ${ambPropertyName}\n";
	$generatedText .= "     *   \\brief  Returns ${ambPropertyName}\n";
	$generatedText .= "     **/\n";

	my $unsigned = '';
	if ( $type =~ m/uint/ ) {
		$unsigned = 'unsigned ';
	}

	if ( $type =~ m/enum/ ) {
		# TODO const unsigned short migth be not enough, guess type based on values
		$generatedText .= "    readonly attribute octet ${ambPropertyName};\n";
	} elsif ( $type =~ m/bool/ ) {
		$generatedText .= "    readonly attribute boolean ${ambPropertyName};\n";
	} elsif ( $type =~ m/int8/ ) {
		$generatedText .= "    readonly attribute ${unsigned}octet ${ambPropertyName};\n";
	} elsif ( $type =~ m/int16/ ) {
		$generatedText .= "    readonly attribute ${unsigned}short ${ambPropertyName};\n";
	} elsif ( $type =~ m/int32/ ) {
		$generatedText .= "    readonly attribute ${unsigned}long ${ambPropertyName};\n";
	} else {
		$generatedText .= "    readonly attribute double ${ambPropertyName};\n";
	}
	$generatedText .= "};\n\n";

	return $generatedText;
}

=head2 encryptAmbPropertyNames

Encrypt AmbPropertyNames.

=cut

sub encryptAmbPropertyNames {
	my $dbcjson = $_[0];

	my @engineControlUnits = @{$dbcjson->{'electronicControlUnits'}};
	for my $ecui (0..scalar(@engineControlUnits)-1) {
		if ( exists( $engineControlUnits[$ecui]{'messages'} ) ) {
			my @messages = @{$engineControlUnits[$ecui]{'messages'}};
			for my $msgi (0..scalar(@messages)-1) {
				my @signals = @{$messages[$msgi]{'signals'}};
				foreach my $signal ( @signals ) {
					my $shownPropertyName = sha1_hex( $signal->{'AMBPropertyName'} );
					$signal->{'AMBPropertyNameEnc'} = 'S' . sha1_hex( $shownPropertyName );
				}
			}
		}
	}
}

=head2 removeDuplicates

Returns array of values witout duplicates.

=cut

sub removeDuplicates {
	my @arr = sort { $a->{'name'} cmp $b->{'name'} }  @{$_[0]};

	my @duplicates;
	my $prev = pop @arr;

	while (defined(my $x = pop @arr)) {
		if ($prev->{'name'} eq $x->{'name'}) {
			push @duplicates, $x;
			while (defined(my $y = pop @arr)) {
				if ($y->{'name'} ne $x->{'name'}) {
					$prev = $y;
					last;
				}
			}
		}
		else {
			$prev = $x;
		}
	}
	# Typically very small arrays
	@arr = sort @{$_[0]};
	if (scalar @duplicates > 0) {
		foreach my $x (@arr) {
			foreach my $y (@duplicates) {
				if ($x->{'name'} eq $y->{'name'}) {
					$x->{'name'} .= '_' . $x->{'value'};
				}
			}
		}
	}

	return @arr;
}

=head2 calculateZone

Returns calculated Zone for given signal.

=cut

sub calculateZone {
	my $ambPropertyName = $_[0];
	my $zone = 'Zone::None';

	if ( $ambPropertyName =~ m/FrL/) {
		$zone = 'Zone::FrontLeft';
	} elsif ( $ambPropertyName =~ m/FrR/) {
		$zone = 'Zone::FrontRight';
	} elsif ( $ambPropertyName =~ m/ReL/) {
		$zone = 'Zone::RearLeft';
	} elsif ( $ambPropertyName =~ m/ReR/) {
		$zone = 'Zone::RearRight';
	}
	return $zone;
}

##############################################################################

=head1 AUTHOR

IntelIVIPoc, C<< <ivipoc at intel.com> >>

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

	perldoc Intel::IVIPoc::AMBPluginGenerator


=head1 ACKNOWLEDGEMENTS

=head1 LICENSE AND COPYRIGHT

Copyright (C) 2012 Intel Corporation

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA


=cut

1; # End of Intel::IviPoc::AmbPluginGenerator
