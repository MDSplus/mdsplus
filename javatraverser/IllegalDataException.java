//package jTraverser;

class IllegalDataException extends Exception {
    Data invalid_data;
    public IllegalDataException(String message, Data invalid_data)
    {
	super(message);
	this.invalid_data = invalid_data;
    }
}