use strict;
use POSIX qw/strftime/;
use TAP::Parser qw/all/;
use TAP::Parser::Aggregator qw/all/;


my @files = @ARGV;
my $aggregate = TAP::Parser::Aggregator->new;
my $test_count = 1;

foreach my $file (@files) { 
      my $parser = TAP::Parser->new( { 
          source => $file,
          sources => { File => { extensions => [ '.tap', '.log', '.t' ] } }
          });
      while ( my $result = $parser->next ) {
        if( $result->is_test ) {
          printf "%s %d %s \n", $result->ok, $test_count, $result->description;
          $test_count += 1;
        }
      }
            

      $aggregate->add($file, $parser);
}

printf "1..%d\n", $aggregate->total;


