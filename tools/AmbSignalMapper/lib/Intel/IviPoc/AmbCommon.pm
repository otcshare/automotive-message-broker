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

package Intel::IviPoc::AmbCommon;
our @ISA = qw(Exporter);
our @EXPORT = qw(evaluateType readFileContent);

use 5.006;
use strict;
use warnings FATAL => 'all';
use Exporter;

=head1 NAME

Intel::IviPoc::AmbCommon - The great new Intel::IviPoc::AmbCommon!

=head1 VERSION

Version 0.01

=cut

our $VERSION = '0.01';


=head1 SYNOPSIS

Module provides subroutines for reading input JSON file and generating Automotive Message Broker
plugin project.

The following little code snippet shows the module usage.

    use Intel::IviPoc::AmbCommon;

    $signal->{'AMBPropertyType'} = &evaluateType($signal);
    ...

=head1 EXPORT

The following convenience methods are provided by this module. They are
exported by default:

=over 4

=item C<processPlugin $signal>

For given signal file name generates the Automotive Message Broker plugin

=back

=head1 SUBROUTINES/METHODS

=head2 evaluateType

Copies all template files into target folder while replacing the name
with newly generated plugin name. Then generates C++ and IDL definitions
based on information comming from input JSON file.

=cut

sub evaluateType {
    my $signal = $_[0];
    my $type = undef;

    if ($signal->{'factor'} == 1) {
        if ($signal->{'length'} == 1) {
            $type = 'bool';
        } else {
            my $count = -1;
            if ( exists( $signal->{'values'} ) ) {
                $count = @{$signal->{'values'}};
            }
            if ($count == (2**$signal->{'length'})) {
                $type = 'enum';
            } else {
                $type = &evalIntType( $signal );
            }
        }
    }
    else {
        if ( $signal->{'factor'} =~ /\.|[eE]/ ) {
            $type = 'double';
        } else {
            $type = &evalIntType( $signal );
        }
    }

return $type;
}

=head2 evalIntType

Copies one template file into target folder while replacing the name
with newly generated plugin name. Then generates definitions
based on information comming from input JSON file.

=cut

sub evalIntType {
    my $signal = $_[0];
    my $type = '';

    my $signedness = '';

    my $min = 0;
    my $max = 0;
    if ($signal->{'signedness'} eq '+') {
        # unsigned
        $min = $signal->{'offset'} + $signal->{'factor'} * 0;
        $max = $signal->{'offset'} + $signal->{'factor'} * ((2**$signal->{'length'} ) - 1);
    }  else {
        # signed
        $min = $signal->{'offset'} - $signal->{'factor'} * (2**$signal->{'length'} - 1);
        $max = $signal->{'offset'} + $signal->{'factor'} * (2**$signal->{'length'} - 1);
    }

    if ( 0 <= $min && $max < (2**8) ) {
        $type = "uint8_t";
    } elsif ( -1*(2**7) <= $min && $max < (2**7) ) {
        $type = "int8_t";
    } elsif ( 0 <= $min && $max < (2**16) ) {
        $type = "uint16_t";
    } elsif ( -1*(2**15) <= $min && $max < (2**15)  ) {
        $type = "int16_t";
    } elsif ( 0 <= $min && $max < (2**32) ) {
        $type = "uint32_t";
    } elsif ( -1*(2**31) <= $min && $max < (2**31)  ) {
        $type = "int32_t";
    } elsif ( 0 <= $min && $max < (2**64) ) {
        $type = "uint64_t";
    } else {
        $type = "int64_t";
    }

    return $type;
}

=head2 readFileContent

Reads and returns the whole content of given file.

=cut

sub readFileContent {
    my $fileName = $_[0];
    open(my $tmpl, '<', $fileName)
        or die "Could not open file '$fileName' $!";

    # Copy data from one file to another.
    my $fileContent = <$tmpl>;

    close $tmpl;

    return $fileContent;
}

##############################################################################

=head1 AUTHOR

IntelIVIPoc, C<< <ivipoc at intel.com> >>

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc Intel::IVIPoc::AmbCommon


=head1 ACKNOWLEDGEMENTS

=head1 LICENSE AND COPYRIGHT

INTEL CONFIDENTIAL
Copyright 2011 - 2013 Intel Corporation All Rights Reserved.

The source code contained or described herein and all documents related to the
source code("Material") are owned by Intel Corporation or its suppliers or
licensors.Title to the Material remains with Intel Corporation or its
suppliers and licensors.The Material may contain trade secrets and proprietary
and confidential information of Intel Corporation and its suppliers and
licensors, and is protected by worldwide copyright and trade secret laws and
treaty provisions.No part of the Material may be used, copied, reproduced,
modified, published, uploaded, posted, transmitted, distributed, or disclosed
in any way without Intels prior express written permission.
No license under any patent, copyright, trade secret or other intellectual
property right is granted to or conferred upon you by disclosure or delivery
of the Materials, either expressly, by implication, inducement, estoppel or
otherwise.Any license under such intellectual property rights must be
express and approved by Intel in writing.

Unless otherwise agreed by Intel in writing, you may not remove or alter this
notice or any other notice embedded in Materials by Intel or Intels suppliers
or licensors in any way.


=cut

1; # End of Intel::IviPoc::AmbCommon
