=head1 NAME

gaussian - Gaussian distribution.

=head1 SYNOPSIS

    x ~ gaussian()
    x ~ gaussian(0.0, 1.0)
    x ~ gaussian(mean = 0.0, std = 1.0)

=head1 DESCRIPTION

A C<gaussian> action specifies a variate that is Gaussian distributed
according to the given C<mean> and C<std> parameters.

=cut

package Bi::Action::gaussian;

use base 'Bi::Model::Action';
use warnings;
use strict;

=head1 PARAMETERS

=over 4

=item C<mean> (position 0, default 0.0)

Mean.

=item C<std> (position 1, default 1.0)

Standard deviation.

=item C<log> (default false)

True for a log-normal distribution, false for a normal distribution. Must be
a constant expression. For clarity, consider using the L<log_normal> or
L<log_gaussian> actions rather than explicitly using this parameter.

=back

=cut
our $ACTION_ARGS = [
  {
    name => 'mean',
    positional => 1,
    default => 0.0
  },
  {
    name => 'std',
    positional => 1,
    default => 1.0
  },
  {
    name => 'log',
    default => 0
  }
];

sub validate {
    my $self = shift;
    
    # HACK: change default of log argument according to class name -- this
    # gets inherited by Bi::Action::log_gaussian etc.
    $ACTION_ARGS->[2]->{default} = int(ref($self) =~ /^Bi\:\:Action\:\:log/);
    
    $self->process_args($ACTION_ARGS);
    $self->set_name('gaussian'); # synonyms exist, standardise name
    $self->ensure_op('~');
    $self->ensure_scalar('mean');
    $self->ensure_scalar('std');
    $self->ensure_scalar('log');
    $self->ensure_const('log');
	$self->set_parent('pdf_');
    $self->set_can_combine(1);
    $self->set_unroll_args(0);
}

sub mean {
    my $self = shift;

    my $mean = $self->get_named_arg('mean');
    my $log = $self->get_named_arg('log')->eval_const;
    if ($log) {
        $mean = new Bi::Expression::Function('exp', [ $mean->clone ]);
    }

    return $mean->simplify;
}

sub std {
    my $self = shift;
    
    my $std = $self->get_named_arg('std')->clone;
    my $log = $self->get_named_arg('log')->eval_const;
    if ($log) {
        my $mean = $self->mean->clone;
        $std = $std*$mean;
    }
    
    return $std->simplify;
}

sub jacobian {
    my $self = shift;

    my $mean = $self->mean;
    my @refs = @{$mean->get_vars};
    my @J = map { $mean->d($_) } @refs;

    return (\@J, \@refs);
}


1;

=head1 AUTHOR

Lawrence Murray <lawrence.murray@csiro.au>

=head1 VERSION

$Rev$ $Date$
