use strict;
use POSIX qw/strftime/;
use TAP::Parser qw/all/;
use TAP::Parser::Aggregator qw/all/;


my @files = @ARGV;
my $aggregate = TAP::Parser::Aggregator->new;
my $test_count = 0;

foreach my $file (@files) { 
   if( -f $file ) {
      my $parser = TAP::Parser->new( { 
          source => $file,
          sources => { File => { extensions => [ '.tap', '.log', '.t' ] } }
          });
      
      while ( my $result = $parser->next ) {
        if( $result->is_test ) {
          $test_count += 1;
          if( $result->has_skip || $result->has_todo ) {
            printf "%s %d %s # %s %s \n", $result->ok, $test_count, $result->description,
                              $result->directive, $result->explanation; }
          else {
            printf "%s %d %s \n", $result->ok, $test_count, $result->description; }
        }
      }            
      $aggregate->add($file, $parser);
    }  
}

printf "1..%d\n", $aggregate->total;
exit($aggregate->failed);
