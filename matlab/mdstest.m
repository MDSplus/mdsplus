
function  result = mdstest( )
% mdstest - connect to a remote mdsplus data server. 
%   
%      This routine is to test the various functions in the MDSplus
%      Matlab/Octave interface.
%
  function result = mdscheck(exp,result_class,result_size,varargin)

    x=mdsvalue(exp,varargin{:});
    if ~strcmp(class(x),result_class)
        display(sprintf('Error %s: expected class %s got %s',exp,result_class,class(x)))
        result = 0;
        return
    end
    try
        if ~all(size(x)==result_size)
            display(sprintf('Error %s: expected size %s got %s',exp,mat2str(result_size),mat2str(size(x))))
            result = 0;
            return
        end
    catch
        display(sprintf('Error %s: expected size %s got %s',exp,mat2str(result_size),mat2str(size(x))))
        result = 0;
        return
    end
    y=mdsvalue('$',x);
    if isa(x,'cell')
      try
        result = strcmp(x,y);
        result = all(result(:));
        if ~result
          display(strcat('Error testing ',exp,' : string array does not match'))
        end
      catch err
        display(strcat('Error testing ',exp,' : ',err.message))
        result=0;
      end
    else
      try
        result = x==y;
        result = all(result(:));
        if ~result
          display(sprintf('Error %s: input != output',exp))
          display(x);
          display(y);
        end
      catch err
        display(strcat('Error testing ',exp,' : ',err.message))
        result = 0;
      end
    end
  end
  try
    info=mdsInfo();
    usePython=info.usePython;
    legacy=info.useLegacy;
  catch
    usePython=false;
    legacy=true;
  end
  if legacy
    single='double';
  else
    single='single';
  end
  result=mdscheck('1BU','uint8',[1,1]);
  result=result && mdscheck('1WU','uint16',[1,1]);
  result=result && mdscheck('1LU','uint32',[1,1]);
  result=result && mdscheck('1QU','uint64',[1,1]);
  result=result && mdscheck('1.',single,[1,1]);
  result=result && mdscheck('1D0','double',[1,1]);
  result=result && mdscheck('"string test"','char',[1,11]);
  result=result && mdscheck('BYTE_UNSIGNED(1 : 100)','uint8',[100,1]);
  result=result && mdscheck('WORD_UNSIGNED(1 : 100)','uint16',[100,1]);
  result=result && mdscheck('LONG_UNSIGNED(1 : 100)','uint32',[100,1]);
  if ~usePython
    result=result && mdscheck('QUADWORD_UNSIGNED(1 : 100)','uint64',[100,1]);
  end
  result=result && mdscheck('BYTE(1 : 100)','int8',[100,1]);
  result=result && mdscheck('WORD(1 : 100)','int16',[100,1]);
  result=result && mdscheck('1 : 100','int32',[100,1]);
  if ~usePython
    result=result && mdscheck('QUADWORD(1 : 100)','int64',[100,1]);
  end
  result=result && mdscheck('set_range(10,2,5,1 : 100)','int32',[10,2,5]);
  result=result && mdscheck('FS_FLOAT(1 : 100)',single,[100,1]);
  result=result && mdscheck('FT_FLOAT(1 : 100)','double',[100,1]);
  result=result && mdscheck('set_range(10,2,5,FT_FLOAT(1 : 100))','double',[10,2,5]);
  if ~legacy
    result=result && mdscheck('["a","b","c","d"]','cell',[4,1]);
    result=result && mdscheck('set_range(2,3,["a","b","c long string","d","e","f"])','cell',[2,3]);
  end
  result=result && mdscheck('$ : $','int32',[100,1],int32(1),int32(100));
  result=result && mdscheck('$ == $','uint8',[1,1],1,2);
  
end
