package RefactorJob;

use strict;
use Trace;
use File::Copy;
use File::Copy::Recursive qw(dirmove);
  
sub new {
    my $class = shift;
    my $self = {
	_item     => shift,
	_base_dir => shift,
	_work_dir => shift,
	_artist   => shift,
	_album    => shift,
	_year     => shift,
    };
      
    &Trace::debug("Created RefactorJob with values:\n\titem:     ".$self->{_item}.".\n\tbase_dir: ".$self->{_base_dir}."\n\twork_dir: ".$self->{_work_dir}."\n\tartist: ".$self->{_artist}."\n");
      
    bless $self, $class;
    return $self;
}

sub create_artist_dir {
    my $self = shift;
    
    my $artist_dir = $self->{_base_dir}."/".$self->{_artist};
    unless (-d $artist_dir) {
	mkdir $artist_dir;
	&Trace::info("Created artist dir: \"$artist_dir\"");
    }
}

sub move_item {
    my $self = shift;
    
    my $artist_dir = $self->{_base_dir}."/".$self->{_artist};

    &Trace::info("Move item: \"".$self->{_item}."\" to: \"".$artist_dir."\/".$self->{_work_dir}."\"");
    dirmove($self->{_item}, "$artist_dir\/$self->{_work_dir}") || die "ERROR: $!";
}

sub get {
    my $self = shift;
    my $var  = shift;
    return $$self{$var};
}


1;

#-----
