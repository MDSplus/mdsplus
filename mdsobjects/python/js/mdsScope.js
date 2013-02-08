var mouseTarget;
var ZOOM = 1;
var CROSSHAIR = 2;
var PAN = 3;
var mode = ZOOM;
//Double click management
var alreadyClicked = false;
var alreadyDoubleClicked = false;

var colorPalette;
var contextMenuSvg; //svg above which righ button has been clicked
var globalShotPanels = new Array(); //Array containing information required for wave panels taking global shot number
var globalShots = new Array(); //Array containing global shot numbers (as defined in the bottom form)

//Limit costants used for  limits management
var LIMITS_XMIN = 1;
var LIMITS_XMAX = 2;
var LIMITS_YMIN = 3;
var LIMITS_YMAX = 4;



var bottomLine = 
    '<form name="scopeMode" >'+
    '<label for="Zoom"><input type="radio" checked = "yes" name="Mode" id="Zoom" onclick="setMode(ZOOM);"/>Zoom</label>'+
    '<label for="Crosshair"> <input type="radio" name="Mode" id="Crosshair" onclick="setMode(CROSSHAIR);"/>Crosshair</label>'+
    '<label for="Pan"><input type="radio" name="Mode" id="Pan" onclick="setMode(PAN);"/> Pan</label>'+
    '<label for="GlobalShots">Shots</label><input type = "text" id="GlobalShots"  onkeydown="if (event.keyCode == 13) {event.preventDefault(); updateGlobalShots();}"/>'+
    '<button type="button"  autofocus id = "UpdateGlobal" onclick="updateGlobalShots()">Update</button><br>'+
    '<label id="Value" />'+
    '</form>';

////////////////Global mouse callback functions///////////////////////

function setMode(inMode)
{
    var idx;
    mode = inMode;
    for(idx = 0; idx < wavePanels.length; idx++)
    {
        wavePanels[idx].notifyMode(inMode);
    }
}

var startZoomX, startZoomY;
var currZoomX, currZoomY;

function contextMenu(e)
{
    e.stopPropagation();
    return false;
}

function mouseDown(svg, e)
{
     var idx;
    e.preventDefault();
    var popupMenu = document.getElementById("ScopePopup"); 
    popupMenu.style.display = 'none';
    e.preventDefault();

    if(event.button == 2) //MB3 (right) button pressed
    {
    // IE is evil and doesn't pass the event object
        if (event == null)
            event = window.event;

        contextMenuSvg = svg;
        var popupMenu = document.getElementById("ScopePopup"); 
        popupMenu.style.display = 'inherit';
        popupMenu.style.left = event.pageX+'px';
        popupMenu.style.top = event.pageY + 'px';
        popupMenu.style.display = 'flex-box';
        e.stopPropagation();
        return false;
        
    }
    //check for double click
    if(!alreadyClicked)
    {
        alreadyClicked = true;
        setTimeout(function(){alreadyClicked = false;}, 300);
    }
    else //it is a double click
    {
        //alert("DOUBLE CLICK");
        for(idx = 0; idx < wavePanels.length; idx++)
        {
            if(wavePanels[idx].svg == svg)
            {
                switch(mode) {
                    case PAN:
                    case ZOOM: 
                        wavePanels[idx].resetScales();
                        break;
                }
            }
        }
        return;
    }
    mouseTarget = e.target;
    for(idx = 0; idx < wavePanels.length; idx++)
    {
        if(wavePanels[idx].svg == svg)
        {
            switch(mode) {
                case ZOOM: 
                    startZoomX = e.offsetX;
                    startZoomY = e.offsetY;
                    wavePanels[idx].startZoom(e.offsetX, e.offsetY);
                    break;
                case CROSSHAIR:
                    wavePanels[idx].startCrosshair(e.offsetX, e.offsetY);
                    break;
                case PAN:
                    wavePanels[idx].startPan(e.offsetX, e.offsetY);
                    break;

            }
         }
    }
    return true;
}
function mouseMove(svg, e)
{
    var idx;
    e.preventDefault();
    if(mouseTarget != e.target)
        return;
    for(idx = 0; idx < wavePanels.length; idx++)
    {
        if(wavePanels[idx].svg == svg)
        {
            switch(mode) {
                case ZOOM: 
                    // Used in case mouse button is released in a different target 
                    currZoomX = e.offsetX;
                    currZoomY = e.offsetY;
                    wavePanels[idx].dragZoom(e.offsetX, e.offsetY);
                    break;
                case CROSSHAIR:
                    wavePanels[idx].dragCrosshair(e.offsetX, e.offsetY);
                    break;
                case PAN:
                    wavePanels[idx].dragPan(e.offsetX, e.offsetY);
                    break;
            }
        }
    }
}
function mouseUp(svg, e)
{
    var idx;
 //   e.preventDefault();
    for(idx = 0; idx < wavePanels.length; idx++)
    {
        if(wavePanels[idx].svg == svg)
        {
            switch(mode) {
                case ZOOM: 
                    if(e.target == mouseTarget) //If released within the same target
                        wavePanels[idx].endZoom(e.offsetX, e.offsetY);
                    else
                        wavePanels[idx].endZoom(currZoomX, currZoomY);
                    break;
                case CROSSHAIR:
                    wavePanels[idx].endCrosshair();
                    break;
                case PAN:
                    wavePanels[idx].endPan();
                    break;
            }
        }
    }
}

//Touch event management 
var touchMoved;
function touchStart(svg, e)
{
    var idx;
    contextMenuSvg = svg;
    e.preventDefault();
    touchMoved = false;
    if(!alreadyClicked)
    {
        alreadyClicked = true;
        setTimeout(function(){alreadyClicked = false;}, 300);
    }
    else //it is a double click
    {
        var popupMenu = document.getElementById("ScopePopup"); 
        popupMenu.style.display = 'inherit';
        popupMenu.style.left = e.touches[0].clientX+'px';
        popupMenu.style.top = e.touches[0].clientY + 'px';
        popupMenu.style.display = 'flex-box';
        e.stopPropagation();
        return ;
    }
    var popupMenu = document.getElementById("ScopePopup"); 
    popupMenu.style.display = 'none';
    e.preventDefault();
    //alert(''+svg.getElementsByTagName("rect")[0].x.value);
    for(idx = 0; idx < wavePanels.length; idx++)
    {
        
        if(wavePanels[idx].svg == svg)
        {
            var touchX = e.touches[0].clientX - wavePanels[idx].col*svg.clientWidth;
            var touchY = e.touches[0].clientY - wavePanels[idx].row*svg.clientHeight;
            switch(mode) {
                case ZOOM: 
                    wavePanels[idx].startZoom(touchX, touchY);
                    break;
                case CROSSHAIR:
                    wavePanels[idx].startCrosshair(touchX, touchY);
                    break;
                case PAN:
                    wavePanels[idx].startPan(touchX, touchY);
                    break;

            }
         }
    }
}

var touchMoveX, touchMoveY;
function touchMove(svg, e)
{
    var idx;
    e.preventDefault();
    touchMoved = true;
    var node = event.target;
    for(idx = 0; idx < wavePanels.length; idx++)
    {
        if(wavePanels[idx].svg == svg)
        {
            touchMoveX = e.touches[0].clientX - wavePanels[idx].col*svg.clientWidth;
            touchMoveY = e.touches[0].clientY- wavePanels[idx].row*svg.clientHeight;
            switch(mode) {
                case ZOOM: 
                    wavePanels[idx].dragZoom(touchMoveX, touchMoveY);
                    break;
                case CROSSHAIR:
                    wavePanels[idx].dragCrosshair(touchMoveX, touchMoveY);
                    break;
                case PAN:
                    wavePanels[idx].dragPan(touchMoveX, touchMoveY);
                    break;
            }
        }
    }
}
function touchEnd(svg, e)
{
    var idx;
    e.preventDefault();
    for(idx = 0; idx < wavePanels.length; idx++)
    {
        if(wavePanels[idx].svg == svg)
        {
            switch(mode) {
                case ZOOM: 
                    if(!touchMoved) wavePanels[idx].clicked(touchMoveX, touchMoveY);
                    wavePanels[idx].endZoom(touchMoveX, touchMoveY);
                    break;
                case CROSSHAIR:
                    wavePanels[idx].endCrosshair();
                    break;
                case PAN:
                    wavePanels[idx].endPan();
                    break;
            }
        }
    }
}
///////////////////////////////////////////////////////////////////////////
/** 
 * Button callback functions and  functions dealing with multiple wave panels (contained in wavePanels array
 **/
function dragOtherCrosshairs(originatorPanel, x, y)
{
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        wavePanels[panelIdx].dragCrosshairExternal(x, y);
    }
}

function autoscale()
{
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
            wavePanels[panelIdx].autoscale();
    }
    var popupMenu = document.getElementById("ScopePopup"); 
    popupMenu.style.display = 'none';
}
function autoscaleAll()
{
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        wavePanels[panelIdx].autoscale();
    }
    var popupMenu = document.getElementById("ScopePopup"); 
    popupMenu.style.display = 'none';
}

function autoscaleY()
{
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
            wavePanels[panelIdx].autoscaleY();
    }
    var popupMenu = document.getElementById("ScopePopup"); 
    popupMenu.style.display = 'none';
}
function allSameScale()
{
    var xMin, xMax, yMin, yMax;
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
        {
            xMin = wavePanels[panelIdx].metrics.xmin;
            xMax = wavePanels[panelIdx].metrics.xmax;
            yMin = wavePanels[panelIdx].metrics.ymin;
            yMax = wavePanels[panelIdx].metrics.ymax;
            break;
        }
    }
    if(xMin == undefined) return;
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg != contextMenuSvg)
        {
            wavePanels[panelIdx].setScale(xMin, xMax, yMin, yMax);
        }
    }
    var popupMenu = document.getElementById("ScopePopup"); 
    popupMenu.style.display = 'none';
}
function allSameXScale()
{
    var xMin, xMax;
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
        {
            xMin = wavePanels[panelIdx].metrics.xmin;
            xMax = wavePanels[panelIdx].metrics.xmax;
            break;
        }
    }
    if(xMin == undefined) return;
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg != contextMenuSvg)
        {
            wavePanels[panelIdx].setScale(xMin, xMax, wavePanels[panelIdx].metrics.ymin, 
                wavePanels[panelIdx].metrics.ymax);
        }
    }
    var popupMenu = document.getElementById("ScopePopup"); 
    popupMenu.style.display = 'none';
}
function allSameXScaleAutoY()
{
    var xMin, xMax;
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
        {
            xMin = wavePanels[panelIdx].metrics.xmin;
            xMax = wavePanels[panelIdx].metrics.xmax;
            break;
        }
    }
    if(xMin == undefined) return;
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg != contextMenuSvg)
        {
            wavePanels[panelIdx].setScaleAutoY(xMin, xMax);
        }
    }
    var popupMenu = document.getElementById("ScopePopup"); 
    popupMenu.style.display = 'none';
}
function allSameYScale()
{
    var yMin, yMax;
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
        {
            yMin = wavePanels[panelIdx].metrics.ymin;
            yMax = wavePanels[panelIdx].metrics.ymax;
            break;
        }
    }
    if(yMin == undefined) return;
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg != contextMenuSvg)
        {
            wavePanels[panelIdx].setScale(wavePanels[panelIdx].metrics.xmin, wavePanels[panelIdx].metrics.xmax, 
                yMin, yMax);
        }
    }
    var popupMenu = document.getElementById("ScopePopup"); 
    popupMenu.style.display = 'none';
}

function resetAllScales()
{
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        wavePanels[panelIdx].resetScales();
    }
    var popupMenu = document.getElementById("ScopePopup"); 
    popupMenu.style.display = 'none';
}

function resetScales()
{
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        if(wavePanels[panelIdx].svg == contextMenuSvg)
            wavePanels[panelIdx].resetScales();
    }
    var popupMenu = document.getElementById("ScopePopup"); 
    popupMenu.style.display = 'none';
}

//////////////Utility function fopr sending expressions in URL encoding
function encodeUrl(inStr)
{   
   return inStr;
 
   var urlStr = inStr.replace("%", "%25"); 
   urlStr = urlStr.replace("/", "%2F"); 
   urlStr = urlStr.replace("#", "%23"); 
   urlStr = urlStr.replace("?", "%3F"); 
   urlStr = urlStr.replace("&", "%24"); 
   urlStr = urlStr.replace("@", "%40"); 
   urlStr = urlStr.replace(":", "%3B"); 
   urlStr = urlStr.replace("+", "%2B"); 
   urlStr = urlStr.replace(" ", "%20");
   return urlStr;
}

 //////////////////////////////////////////////////////////////////////////
/**
*  Class Signal conytains Signal-related information including color and marker mode
**/
var PLOT_LINE = 1;
var PLOT_POINT = 2;
var PLOT_LINE_POINT = 3;

function Signal(x, y, color, mode)
{
    this.x = x;
    this.y = y;
    this.size = x.length;
    if(this.size > y.length)
        this.size = y.length;
    if(color == undefined)
        this.color = 'black';
    else
        this.color = color;
    if(mode == undefined)
        this.mode = PLOT_LINE;
    else
        this.mode = mode;
    
    function getClosestIdx(x)
    {
        var idx;
        if(x <= this.x[0])
            return 0;
        if(x >= this.x[this.size - 1])
            return this.size - 1;
        for(idx = 0; idx < this.size - 1; idx++)
            if(x >= this.x[idx] && x <= this.x[idx+1])
                return idx;
    }
    this.getClosestIdx = getClosestIdx;
    
    function getYDist(idx, y)
    {
        return Math.abs(y - this.y[idx]);
    }
    this.getYDist = getYDist;
    
    function getMinX()
    {
        var idx;
        var xmin = this.x[0];
        for(idx = 0; idx < this.size; idx++)
        {
            if(this.x[idx] < xmin)
                xmin = this.x[idx]
        }
        return xmin;
    }
    this.getMinX = getMinX;
    function getMaxX()
    {
        var idx;
        var xmax = this.x[0];
        for(idx = 0; idx < this.size; idx++)
        {
            if(this.x[idx] > xmax)
                xmax = this.x[idx]
        }
        return xmax;
    }
    this.getMaxX = getMaxX;
    function getMinY()
    {
        var idx;
        var ymin = this.y[0];
        for(idx = 0; idx < this.size; idx++)
        {
            if(this.y[idx] < ymin)
                ymin = this.y[idx]
        }
        return ymin;
    }
    this.getMinY = getMinY;
    function getMaxY()
    {
        var idx;
        var ymax = this.y[0];
        for(idx = 0; idx < this.size; idx++)
        {
            if(this.y[idx] > ymax)
                ymax = this.y[idx]
        }
        return ymax;
    }
    this.getMaxY = getMaxY;
    function getMinYInRange(x1, x2)
    {
        var idx;
        for(idx = 0; idx < this.size && this.x[idx] < x1; idx++);
        if(idx == this.size) return 0; //Default interval 0-1 returned when outside signals
        var ymin = this.y[idx];
        for(; idx < this.size && this.x[idx] <= x2; idx++)
        {
            if(this.y[idx] < ymin)
                ymin = this.y[idx]
        }
        return ymin;
        
    }
    this.getMinYInRange = getMinYInRange;
    function getMaxYInRange(x1, x2)
    {
        var idx;
        for(idx = 0; idx < this.size && this.x[idx] < x1; idx++);
        if(idx == this.size) return 1; //Default interval 0-1 returned when outside signals
        var ymax = this.y[idx];
        for(; idx < this.size && this.x[idx] <= x2; idx++)
        {
            if(this.y[idx] > ymax)
                ymax = this.y[idx]
        }
        return ymax;
        
    }
    this.getMaxYInRange = getMaxYInRange;
}
 
/**
*  Class Metrics handles mapping between waveform coordinates and pixel coordinates
**/
function Metrics(marginPix, width, height, xMin, xMax, yMin, yMax)
{
    this.width = width;
    this.height = height;
    this.marginPix = marginPix;
    this.xmin = xMin;
    this.xmax = xMax;
    this.ymin = yMin;
    this.ymax = yMax;
    this.xfact = (this.width - 2 * this.marginPix)/(this.xmax - this.xmin);
    this.yfact = (this.height - 2 * this.marginPix)/(this.ymax - this.ymin);

   
    function getXPixel(xVal)
    {
        var xPixel = Math.round(this.marginPix + this.xfact * (xVal - this.xmin));
	return xPixel;
        if(xPixel >= 0 && xPixel <= this.width)
            return xPixel;
        else
            return undefined;
    }
    this.getXPixel = getXPixel;
    function getYPixel(yVal)
    {
        var yPixel = Math.round(this.height -this.marginPix - this.yfact * (yVal - this.ymin));
	return yPixel;
/*        if(yPixel >= 0 && yPixel <= this.height)
            return yPixel;
        else
            return undefined;
*/    }
    this.getYPixel = getYPixel;
    function getXValue(xPixel)
    {
        return this.xmin + (xPixel - this.marginPix)/this.xfact;
    }
    this.getXValue = getXValue;
    function getYValue(yPixel)
    {
        return this.ymin + (this.height - yPixel - this.marginPix)/this.yfact;
    }
    this.getYValue = getYValue;
}

function Grid(g, metrics, labels)
{
    this.numSteps = 8;
    this.xmin = metrics.xmin;
    this.xmax = metrics.xmax;
    this.ymin = metrics.ymin;
    this.ymax = metrics.ymax;
    this.width = metrics.width;
    this.height = metrics.height;
    this.metrics = metrics;
    this.labels = labels;
    this.g = g;
    
    function buildGrid(minValue, maxValue)
    {
        var i;
        if (maxValue < minValue)
            maxValue = minValue + 1E-10;

        var range = maxValue - minValue;
        if (range <= 0)
            range = 1E-3;

        var currMaxValue = maxValue;// + 0.1 * range;
        var currMinValue = minValue;// - 0.1 * range;
        var step = (maxValue - minValue) / this.numSteps;

        var greater;
        var count = 0;
        if (step > 1)
        {
            greater = true;
            while (step / 10. > 1.)
            {
                step /= 10;
                count++;
            }
        }
        else
        {
            greater = false;
            while (step > 0 && step < 1)
            {
                step *= 10;
                count++;
            }
        }
        step = Math.round(step);
        if (greater)
            for (i = 0; i < count; i++)
                step *= 10;
        else
            for (i = 0; i < count; i++)
                step /= 10;
        
        var curr = Math.round(currMinValue / step) * step;
       // while (curr >= currMinValue)
       //     curr -= step;
        
        var retVals = new Array();
//        while(curr < currMaxValue + step)
        while(curr < currMaxValue)
        {
            var currVal = Math.round(curr / step) * step;
//Fix for 0 value
            if (currVal < step / 100. && currVal > -step / 100.)
                currVal = 0;
            retVals.push(currVal);
            curr += step;
        }
        return retVals;
    }
    this.buildGrid = buildGrid;
    this.xVals = this.buildGrid(this.xmin, this.xmax);
    this.yVals = this.buildGrid(this.ymin, this.ymax);
    
    function roundLabel(label)
    {
 //       var dotPos = label.indexOf(".");
 //       if(dotPos == -1 || (label.length - dotPos) < 8)
 //           return label;
        var rounded = Math.round(parseFloat(label)*100000)/100000;
        if((Math.abs(rounded) > 0.0001 && Math.abs(rounded) < 10000)||rounded == 0)
            return ''+rounded;
        else
            return rounded.toExponential();
            
     }
    this.roundLabel = roundLabel;
    
    function plot()
    {
        for (var idx=0; idx < this.xVals.length; idx++) {
            if(this.metrics.getXPixel(this.xVals[idx]) > this.width / 30)
            {
            var label=document.createElementNS("http://www.w3.org/2000/svg","text");
            this.g.appendChild(label);
            label.setAttributeNS(null,"text-anchor","start");
            //label.setAttributeNS(null,"x",this.xScaling[1]+idx * this.xScaling[2]);
            label.setAttributeNS(null,"x",this.metrics.getXPixel(this.xVals[idx]));
            if(this.labels.xlabel != undefined)
                label.setAttributeNS(null,"y",this.height - 20);
            else
                label.setAttributeNS(null,"y",this.height - 10);
            label.setAttributeNS(null,"font-size","16px");
            var textNode=document.createTextNode(this.roundLabel(this.xVals[idx]+''));
            label.appendChild(textNode);
            }
            
            var lineY=document.createElementNS("http://www.w3.org/2000/svg","line");
            lineY.setAttribute("y1","0");
            lineY.setAttribute("y2",''+this.height);
 //           lineY.setAttribute("x1",this.width-(this.xScaling[1]+idx * this.xScaling[2]));
 //           lineY.setAttribute("x2",this.width-(this.xScaling[1]+idx * this.xScaling[2]));
            lineY.setAttribute("x1",this.metrics.getXPixel(this.xVals[idx]));
            lineY.setAttribute("x2",this.metrics.getXPixel(this.xVals[idx]));
            lineY.setAttribute("stroke","black");
            lineY.setAttribute("stroke-width","1");
            lineY.setAttribute("stroke-dasharray", "1,4");
            g.appendChild(lineY);

            
            
        }
        for (var idx=0; idx < this.yVals.length; idx++) {
            var label=document.createElementNS("http://www.w3.org/2000/svg","text");
            this.g.appendChild(label);
            label.setAttributeNS(null,"text-anchor","start");
            if(this.labels.ylabel != null)
                label.setAttributeNS(null,"x",18);
            else
                label.setAttributeNS(null,"x",10);
            label.setAttributeNS(null,"y",this.metrics.getYPixel(this.yVals[idx]));
            label.setAttributeNS(null,"font-size","16px");
            label.setAttribute("alignment-baseline", "middle");
            //var value=Round(this.yScaling[3]+idx * this.yScaling[4], this.yScaling[5]);
            var textNode=document.createTextNode(this.roundLabel(this.yVals[idx]+''));
            //var textNode=document.createTextNode(value.toFixed(9));
             label.appendChild(textNode);
            var lineX=document.createElementNS("http://www.w3.org/2000/svg","line");
            lineX.setAttribute("x1","0");
            lineX.setAttribute("x2",''+this.width);
//            lineX.setAttribute("y1",this.height-(this.yScaling[1]+idx * this.yScaling[2]));
//            lineX.setAttribute("y2",this.height-(this.yScaling[1]+idx * this.yScaling[2]));
            lineX.setAttribute("y1",this.metrics.getYPixel(this.yVals[idx]));
            lineX.setAttribute("y2",this.metrics.getYPixel(this.yVals[idx]));
            lineX.setAttribute("stroke","black");
            lineX.setAttribute("stroke-width","1");
            lineX.setAttribute("stroke-dasharray", "1,4");
            g.appendChild(lineX);
        }
        if(this.labels.title != undefined)
        {
            var title=document.createElementNS("http://www.w3.org/2000/svg","text");
            this.g.appendChild(title);
            title.setAttributeNS(null,"text-anchor","middle");
            title.setAttributeNS(null,"x",this.metrics.width/2);
            title.setAttributeNS(null,"y",18);
            title.setAttributeNS(null,"font-size","18px");
            //title.setAttributeNS(null,"alignment-baseline", "text-after-edge");
            title.appendChild(document.createTextNode(this.labels.title));
        }
        if(this.labels.ylabel != undefined)
        {
            var ylabel=document.createElementNS("http://www.w3.org/2000/svg","text");
            this.g.appendChild(ylabel);
            ylabel.setAttributeNS(null,"text-anchor","middle");
            ylabel.setAttributeNS(null,"y",this.metrics.height/2);
            ylabel.setAttributeNS(null,"x",0);
            ylabel.setAttribute("transform", "rotate(270, "+16+","+this.metrics.height/2+")"), 
            ylabel.setAttributeNS(null,"font-size","16px");
            //title.setAttributeNS(null,"alignment-baseline", "text-after-edge");
            ylabel.appendChild(document.createTextNode(this.labels.ylabel));
        }
        if(this.labels.xlabel != undefined)
        {
            var xlabel=document.createElementNS("http://www.w3.org/2000/svg","text");
            this.g.appendChild(xlabel);
            xlabel.setAttributeNS(null,"text-anchor","middle");
            xlabel.setAttributeNS(null,"y",this.metrics.height - 5);
            xlabel.setAttributeNS(null,"x",this.metrics.width/2);
             xlabel.setAttributeNS(null,"font-size","16px");
            //title.setAttributeNS(null,"alignment-baseline", "text-after-edge");
            xlabel.appendChild(document.createTextNode(this.labels.xlabel));
        }
    }
    this.plot = plot;
}


function Wave(signals, color, g, metrics)
{
    this.signals = signals;
    this.g = g;
    this.color = color;
    this.metrics = metrics;
    function plot()
    {
        var idx;
        var signalIdx;
        for(signalIdx = 0; signalIdx < this.signals.length; signalIdx++)
        {
            if(this.signals[signalIdx].mode == PLOT_LINE || this.signals[signalIdx].mode == PLOT_LINE_POINT)
            {    
                var path=document.createElementNS("http://www.w3.org/2000/svg","path");
                path.setAttribute("fill","none");
                path.setAttribute("stroke",this.signals[signalIdx].color);
                path.setAttribute("stroke-width","1");
                g.appendChild(path);
                this.path = path;
                var ans='M';
                var prevSkipped = false;
		var prevXs = this.metrics.getXPixel(this.signals[signalIdx].x[0]);
		var minYs = this.metrics.getYPixel(this.signals[signalIdx].y[0]);
		var maxYs = minYs;
                for (idx=0; idx < this.signals[signalIdx].size; idx++) {
                    var xs=this.metrics.getXPixel(this.signals[signalIdx].x[idx]);
                    var ys=this.metrics.getYPixel(this.signals[signalIdx].y[idx]);
		    if(ys == undefined || xs == undefined) continue;
		    if(idx > 0 && xs == prevXs)
		    {
			if(ys < minYs) minYs = ys;
			if(ys > maxYs) maxYs = ys;
		    }
		    else
		    {
		        if(minYs == undefined)
			{
			    minYs = ys;
			    maxYs = us;
			}
//			ans = ans + ' ' + xs + ' ' +minYs;
			ans = ans + ' ' + xs + ' ' +ys;
			if (maxYs > minYs)
			{
			   ans = ans + ' ' + xs + ' ' +maxYs;
			}
			minYs = ys;
			maxYs = ys;
			prevXs = xs;
		    } 

/*                    if((xs == undefined) || (ys == undefined))
                        prevSkipped = true;
                    else
                    {
                        if(prevSkipped && ans != 'M')
                            ans = ans + " M " + xs + " " + ys;
                        else
                            ans = ans + " " + xs + " " + ys;
                        prevSkipped = false;
                    }
 
*/              }
                path.setAttribute("d",ans);
            }
            if(this.signals[signalIdx].mode == PLOT_POINT || this.signals[signalIdx].mode == PLOT_LINE_POINT)
            {  
                for (idx=0; idx < this.signals[signalIdx].size; idx++) {
                    var xs=this.metrics.getXPixel(this.signals[signalIdx].x[idx]);
                    var ys=this.metrics.getYPixel(this.signals[signalIdx].y[idx]);
                    if((xs != undefined) && (ys != undefined))
                    {
                        var circle=document.createElementNS("http://www.w3.org/2000/svg","circle");
                        circle.setAttribute("cx",xs);
                        circle.setAttribute("cy",ys);
                        circle.setAttribute("r",2);
                        circle.setAttribute("fill",this.signals[signalIdx].color);
                        g.appendChild(circle);
                    }
                }
            }  
        }
    }
    this.plot = plot;
    
    
    function getCrosshairIdx(xVal, y)
    {
        var minSigIdx;
        var minDist;
        var idx;
        for(idx = 0; idx < this.signals.length; idx++)
        {
            var currMinIdx = this.signals[idx].getClosestIdx(xVal);
            var currDist = this.signals[idx].getYDist(currMinIdx, y);
            if(minDist == undefined || minDist > currDist)
            {
                minSigIdx = idx;
                minDist = currDist;
            }
        }
        return minSigIdx;
    }
    this.getCrosshairIdx = getCrosshairIdx;

    function getCrosshairY(xVal, sigIdx)
    {
        return this.signals[sigIdx].y[this.signals[sigIdx].getClosestIdx(xVal)];
    }
    this.getCrosshairY = getCrosshairY;
    function getCrosshairColor(sigIdx)
    {
        return this.signals[sigIdx].color;
    }
    this.getCrosshairColor = getCrosshairColor;
}    

function WavePanel(signals,svg, numCols, numRows, col, row, labels)
{
    this.STARTING_ZOOM = 1;
    this.ZOOMING = 2;
    this.NOT_ZOOMING = 3;
    this.zoomState = this.NOT_ZOOMING;
    this.svg = svg;
    this.signals = signals;
    this.numCols = numCols;
    this.numRows = numRows;
    this.col = col;
    this.row = row;
    this.labels = labels;
    var g=document.createElementNS("http://www.w3.org/2000/svg","g");
    g.setAttribute("id","viewport");
    svg.appendChild(g);
    this.g = g;


    this.borderRect=document.createElementNS("http://www.w3.org/2000/svg","rect");
    this.borderRect.setAttribute("x","0");
    this.borderRect.setAttribute("y","0");
    this.borderRect.setAttribute("width",svg.viewBox.baseVal.width);
    this.borderRect.setAttribute("height",svg.viewBox.baseVal.height);
    this.borderRect.setAttribute("fill","none");
    this.borderRect.setAttribute("stroke","black");
    svg.appendChild(this.borderRect);
  
    //default limits wehn no signal
    this.xMin = 0;
    this.xMax = 1;
    this.yMin = 0;
    this.yMax = 1;
    //find min and max X and Y values of the set of signals
    var sigIdx;
    
    for(sigIdx = 0; sigIdx < signals.length; sigIdx++)
    {
        var currXMin = this.signals[sigIdx].getMinX();
        if(this.xMin == undefined || currXMin < this.xMin)
            this.xMin = currXMin;
        var currXMax = this.signals[sigIdx].getMaxX();
        if(this.xMax == undefined || currXMax > this.xMax)
            this.xMax = currXMax;
        var currYMin = this.signals[sigIdx].getMinY();
        if(this.yMin == undefined || currYMin < this.yMin)
            this.yMin = currYMin;
        var currYMax = this.signals[sigIdx].getMaxY();
        if(this.yMax == undefined || currYMax > this.yMax)
            this.yMax = currYMax;
    }
    var metrics = new Metrics(30, svg.viewBox.baseVal.width, svg.viewBox.baseVal.height, 
        this.xMin, this.xMax, this.yMin, this.yMax);
    var wave = new Wave(this.signals, 0, this.g, metrics);
    var grid = new Grid(g, metrics, this.labels);
    this.metrics = metrics;
    this.wave = wave;
    this.grid = grid;

    function pushSignal(signal)
    {
        this.signals.push(signal);
        this.svg.removeChild(this.g);
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.svg.appendChild(this.g);

        var currXMin = signal.getMinX();
        if(this.xMin == undefined || currXMin < this.xMin)
            this.xMin = currXMin;
        var currXMax = signal.getMaxX();
        if(this.xMax == undefined || currXMax > this.xMax)
            this.xMax = currXMax;
        var currYMin = signal.getMinY();
        if(this.yMin == undefined || currYMin < this.yMin)
            this.yMin = currYMin;
        var currYMax = signal.getMaxY();
        if(this.yMax == undefined || currYMax > this.yMax)
            this.yMax = currYMax;

        var xmin, xmax, ymin, ymax;
         
        if(this.actXMin == undefined)
            xmin = this.xMin;
        else
            xmin = this.actXMin;
        if(this.actXMax == undefined)
            xmax = this.xMax;
        else
            xmax = this.actXMax;
        if(this.actYMin == undefined)
            ymin = this.yMin;
        else
            ymin = this.actYMin;
        if(this.actYMax == undefined)
            ymax = this.yMax;
        else
            ymax = this.actYMax;
        
         var metrics = new Metrics(30, svg.viewBox.baseVal.width, svg.viewBox.baseVal.height, 
            xmin, xmax, ymin, ymax);
        var wave = new Wave(this.signals, 0, this.g, metrics);
        var grid = new Grid(this.g, metrics, this.labels);
        this.metrics = metrics;
        this.wave = wave;
        this.grid = grid;

    }
    this.pushSignal = pushSignal;

    function removeAllSignals()
    {
        this.signals = new Array();
        this.svg.removeChild(this.g);
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.svg.appendChild(this.g);
        this.xMin = undefined;
        this.xMax = undefined;
        this.yMin = undefined;
        this.yMax = undefined;
        var metrics = new Metrics(30, svg.viewBox.baseVal.width, svg.viewBox.baseVal.height, 
            0., 1. ,0., 1.);
        var wave = new Wave(signals, 0, this.g, metrics);
        var grid = new Grid(this.g, metrics, this.labels);
        this.metrics = metrics;
        this.wave = wave;
        this.grid = grid;
        this.plot();
    }
    this.removeAllSignals = removeAllSignals;

    function setTitle(title)
    {
        this.labels.title = title;
        this.svg.removeChild(this.g);
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        //this.metrics = new Metrics(30, width/this.numCols, height/this.numRows, this.metrics.xmin, 
        //        this.metrics.xmax, this.metrics.ymin, this.metrics.ymax);
        this.wave = new Wave(this.signals, this.colors, this.g, this.metrics);
        this.grid = new Grid(this.g, this.metrics, this.labels)
        this.plot();
     }
     this.setTitle = setTitle;

    
    function setLimits(limits)
    {
        if(limits.xmin != undefined)
            this.actXMin = limits.xmin;
        if(limits.xmax != undefined)
            this.actXMax = limits.xmax;
        if(this.actXMax != undefined && this.actXMax != undefined && this.actXMax < this.actXMin)
            this.actXMax = this.actXMin + 1E-9;
        if(limits.ymin != undefined)
            this.actYMin = limits.ymin;
        if(limits.ymax != undefined)
            this.actYMax = limits.ymax;
        if(this.actYMax != undefined && this.actYMax != undefined && this.actYMax < this.actYMin)
            this.axtYMax = this.actYMin + 1E-9;

        var xmin, xmax, ymin, ymax;
        if(this.actXMin == undefined)
            xmin = this.xMin;
        else
            xmin = this.actXMin;
        if(this.actXMax == undefined)
            xmax = this.xMax;
        else
            xmax = this.actXMax;
        if(this.actYMin == undefined)
            ymin = this.yMin;
        else
            ymin = this.actYMin;
        if(this.actYMax == undefined)
            ymax = this.yMax;
        else
            ymax = this.actYMax;
        
        this.svg.removeChild(this.g);
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(30, svg.viewBox.baseVal.width, svg.viewBox.baseVal.height, 
            xmin, xmax, ymin, ymax);
        this.wave = new Wave(this.signals, 0, this.g, this.metrics);
        this.grid = new Grid(this.g, this.metrics, this.labels);
    }
    this.setLimits = setLimits;

    function plot()
    {
        this.grid.plot();
        this.wave.plot();
    }
    this.plot = plot;
    
    function resize(width, height)
    {
        this.svg.setAttributeNS(null,"width",width/this.numCols);
        this.svg.setAttributeNS(null,"height",height/this.numRows);
        this.svg.setAttribute("viewBox","0 0 "+width/this.numCols +" " +height/this.numRows);
        this.svg.removeChild(this.borderRect);
        this.borderRect=document.createElementNS("http://www.w3.org/2000/svg","rect");
        this.borderRect.setAttribute("x","0");
        this.borderRect.setAttribute("y","0");
        this.borderRect.setAttribute("width",svg.viewBox.baseVal.width);
        this.borderRect.setAttribute("height",svg.viewBox.baseVal.height);
        this.borderRect.setAttribute("fill","none");
        this.borderRect.setAttribute("stroke","black");
        svg.appendChild(this.borderRect);
        this.svg.removeChild(this.g);
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(30, width/this.numCols, height/this.numRows, this.metrics.xmin, 
                this.metrics.xmax, this.metrics.ymin, this.metrics.ymax);
        this.wave = new Wave(this.signals, this.colors, this.g, this.metrics);
        this.grid = new Grid(this.g, this.metrics, this.labels)
        this.plot();
    }
    this.resize = resize;
    
    function notifyMode(inMode)
    {
        if(inMode != CROSSHAIR)
        {
           if(this.crosshairVertLine != undefined)
           {
                this.g.removeChild(this.crosshairVertLine);   
                this.crosshairVertLine = undefined;
           }
           if(this.crosshairHorLine != undefined)
           {
                this.g.removeChild(this.crosshairHorLine);   
                this.crosshairHorLine = undefined;
           }
        }
    }
    this.notifyMode = notifyMode;
    
    function startCrosshair(x,y)
    {
        this.crosshairing = true;
        var xVal = this.metrics.getXValue(x);
        var yVal = this.metrics.getYValue(y);
        this.crosshairIdx = this.wave.getCrosshairIdx(xVal, yVal);
        this.crosshairColor = this.wave.getCrosshairColor(this.crosshairIdx);
        var crossY = this.wave.getCrosshairY(xVal, this.crosshairIdx);
        var yPixel = this.metrics.getYPixel(crossY);
        if(this.crosshairVertLine == undefined)
            this.crosshairVertLine = document.createElementNS("http://www.w3.org/2000/svg","line");
        this.crosshairVertLine.setAttribute("x1", x);
        this.crosshairVertLine.setAttribute("x2", x);
        this.crosshairVertLine.setAttribute("y1", 0);
        this.crosshairVertLine.setAttribute("y2", this.svg.viewBox.baseVal.height);
        this.crosshairVertLine.setAttribute("stroke", this.crosshairColor);
        this.crosshairVertLine.setAttribute("stroke-width","1");
        this.g.appendChild(this.crosshairVertLine);
        if(this.crosshairHorLine == undefined)
            this.crosshairHorLine = document.createElementNS("http://www.w3.org/2000/svg","line");
        this.crosshairHorLine.setAttribute("x1", 0);
        this.crosshairHorLine.setAttribute("x2", this.svg.viewBox.baseVal.width);
        this.crosshairHorLine.setAttribute("y1", yPixel);
        this.crosshairHorLine.setAttribute("y2", yPixel);
        this.crosshairHorLine.setAttribute("stroke",this.crosshairColor);
        this.crosshairHorLine.setAttribute("stroke-width","1");
        this.g.appendChild(this.crosshairHorLine);
    }
    this.startCrosshair = startCrosshair;
    
    function dragCrosshair(x,y)
    {
        if(this.crosshairing == undefined || this.crosshairing == false) 
            return;
        var xVal = this.metrics.getXValue(x);
        var yVal = this.wave.getCrosshairY(xVal, this.crosshairIdx);
        var crossY = this.wave.getCrosshairY(xVal, this.crosshairIdx);
        var yPixel = this.metrics.getYPixel(crossY);
        this.crosshairVertLine.setAttribute("x1", x);
        this.crosshairVertLine.setAttribute("x2", x);
        this.crosshairVertLine.setAttribute("y1", 0);
        this.crosshairVertLine.setAttribute("y2", this.svg.viewBox.baseVal.height);
        this.crosshairHorLine.setAttribute("x1", 0);
        this.crosshairHorLine.setAttribute("x2", this.svg.viewBox.baseVal.width);
        this.crosshairHorLine.setAttribute("y1", yPixel);
        this.crosshairHorLine.setAttribute("y2", yPixel);
        document.getElementById("Value").innerHTML = 'X: '+(Math.round(xVal * 100000)/100000) +'   Y: '+Math.round(yVal * 100000)/100000;
        dragOtherCrosshairs(this, xVal, yVal);  
    }
    this.dragCrosshair = dragCrosshair;
    //Called when crosshair is dragged in another panel
    function dragCrosshairExternal(xVal, yVal)
    {
        if(this.crosshairing)
            return;
        var xPixel = this.metrics.getXPixel(xVal);
        var yPixel = this.metrics.getYPixel(yVal);
        this.crosshairColor = this.wave.getCrosshairColor(0);
        if(this.crosshairVertLine == undefined)
        {
            this.crosshairVertLine = document.createElementNS("http://www.w3.org/2000/svg","line");
            this.crosshairVertLine.setAttribute("x1", xPixel);
            this.crosshairVertLine.setAttribute("x2", xPixel);
            this.crosshairVertLine.setAttribute("y1", 0);
            this.crosshairVertLine.setAttribute("y2", this.svg.viewBox.baseVal.height);
            this.crosshairVertLine.setAttribute("stroke", this.crosshairColor);
            this.crosshairVertLine.setAttribute("stroke-width","1");
            this.g.appendChild(this.crosshairVertLine);
        }
        else
        {
            this.crosshairVertLine.setAttribute("x1", xPixel);
            this.crosshairVertLine.setAttribute("x2", xPixel);
            this.crosshairVertLine.setAttribute("y1", 0);
            this.crosshairVertLine.setAttribute("y2", this.svg.viewBox.baseVal.height);
        }
        if(this.crosshairHorLine == undefined)
        {
            this.crosshairHorLine = document.createElementNS("http://www.w3.org/2000/svg","line");
            this.crosshairHorLine.setAttribute("x1", 0);
            this.crosshairHorLine.setAttribute("x2", this.svg.viewBox.baseVal.width);
            this.crosshairHorLine.setAttribute("y1", yPixel);
            this.crosshairHorLine.setAttribute("y2", yPixel);
            this.crosshairHorLine.setAttribute("stroke",this.crosshairColor);
            this.crosshairHorLine.setAttribute("stroke-width","1");
            this.g.appendChild(this.crosshairHorLine);
        }
        else
        {
            this.crosshairHorLine.setAttribute("x1", 0);
            this.crosshairHorLine.setAttribute("x2", this.svg.viewBox.baseVal.width);
            this.crosshairHorLine.setAttribute("y1", yPixel);
            this.crosshairHorLine.setAttribute("y2", yPixel);
        }
    }
    this.dragCrosshairExternal = dragCrosshairExternal;
    
    function endCrosshair()
    {
        this.crosshairing = false;
    }
    this.endCrosshair = endCrosshair;
    
    function startPan(x,y)
    {
        this.startPanXVal = this.metrics.getXValue(x);
        this.startPanYVal = this.metrics.getYValue(y);
        this.originalPanMetrics = this.metrics;
        this.panning = true;
    }
    this.startPan = startPan;
    
    function dragPan(x, y)
    {
        if(this.panning == undefined || this.panning == false) return;
        var currXVal = this.originalPanMetrics.getXValue(x);
        var currYVal = this.originalPanMetrics.getYValue(y);
        var deltaX = this.startPanXVal - currXVal;
        var deltaY = this.startPanYVal - currYVal;
        this.metrics = new Metrics(30, this.originalPanMetrics.width, this.originalPanMetrics.height, 
          this.originalPanMetrics.xmin + deltaX, this.originalPanMetrics.xmax + deltaX, 
          this.originalPanMetrics.ymin + deltaY,  this.originalPanMetrics.ymax + deltaY);
        this.svg.removeChild(this.g);
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.wave = new Wave(this.wave.signals, 0, this.g, this.metrics);
        this.grid = new Grid(this.g, this.metrics, this.labels)
        this.plot();
    }
    this.dragPan = dragPan;
    
    function endPan()
    {
        this.panning = false;
    }
    this.endPan = endPan;
    
    function startZoom(x,y)
    {
        this.zoomState = this.STARTING_ZOOM;
        if(this.crosshairVertLine != undefined)
        {
            this.g.removeChild(this.crosshairVertLine);
            this.crosshairVertLine = undefined;
        }
        if(this.crosshairHorLine != undefined)
        {
            this.g.removeChild(this.crosshairHorLine);
            this.crosshairHorLine = undefined;
        }
    }
    this.startZoom = startZoom;
    function beginZoom(x, y)
    {
        this.svg.style.cursor = "crosshair";
        if(this.zoomRect)
            this.g.removeChild(this.zoomRect);
        this.zoomX = x;
        this.zoomY = y;
        var zoomRect = document.createElementNS("http://www.w3.org/2000/svg","rect");
        zoomRect.setAttribute("x",x);
        zoomRect.setAttribute("y",y);
        zoomRect.setAttribute("width","1");
        zoomRect.setAttribute("height","1");
        zoomRect.setAttribute("fill","none");
        zoomRect.setAttribute("stroke","black");
        zoomRect.setAttribute("stroke-width","1");
        this.zoomRect = zoomRect;
        this.g.appendChild(zoomRect);
    }
    this.beginZoom = beginZoom;
    function dragZoom(x, y)
    {
        if(this.zoomState == this.NOT_ZOOMING)
            return;
        if(this.zoomState == this.STARTING_ZOOM)
        {
            this.zoomState = this.ZOOMING;
            this.beginZoom(x, y);
        }
        var actWidth, actHeight, actX, actY;
        this.svg.style.cursor = "crosshair";
        var currWidth = x - this.zoomX;
        var currHeight = y - this.zoomY;
        if(this.zoomRect)
        {
            if(currWidth > 0)
            {
                actX = this.zoomX;
                actWidth = currWidth;
            }
            else
            {
                actX = x;
                actWidth = -currWidth;
            }
            if(currHeight > 0)
            {
                actY = this.zoomY;
                actHeight = currHeight;
            }
            else
            {
                actY = y;
                actHeight = -currHeight;
            }
            this.zoomRect.setAttribute("x",actX);
            this.zoomRect.setAttribute("y",actY);
            this.zoomRect.setAttribute("width",actWidth);
            this.zoomRect.setAttribute("height",actHeight);
        }
    }
    this.dragZoom = dragZoom;
    function endZoom(x, y)
    {
        if(this.zoomState != this.ZOOMING)
        {
            this.zoomState = this.NOT_ZOOMING;
            return;
        }
        this.zoomState = this.NOT_ZOOMING;
        if(this.zoomRect)
            this.g.removeChild(this.zoomRect);
        this.zoomRect = undefined;
        //Check for 'fake' zooms (i.e. selecting an area which smaller in width or height tham 1/100 of the window size)'
        if(Math.abs(x - this.zoomX) < this.metrics.width/100 || Math.abs(y - this.zoomY) < this.metrics.height/100)
            return;
        var xMin, xMax, yMin, yMax;
        if(x < this.zoomX)
        {
            xMin = this.metrics.getXValue(x);
            xMax = this.metrics.getXValue(this.zoomX);
        }
        else
        {
            xMin = this.metrics.getXValue(this.zoomX);
            xMax = this.metrics.getXValue(x);
        }
        if(y > this.zoomY)
        {
            yMin = this.metrics.getYValue(y);
            yMax = this.metrics.getYValue(this.zoomY);
        }
        else
        {
            yMin = this.metrics.getYValue(this.zoomY);
            yMax = this.metrics.getYValue(y);
        }
        this.svg.removeChild(this.g);
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(30, this.metrics.width, this.metrics.height, xMin, xMax, yMin, yMax);
        this.wave = new Wave(this.signals, 0, this.g, this.metrics);
        this.grid = new Grid(this.g, this.metrics, this.labels)
        this.plot();
    }
    this.endZoom = endZoom;
    function autoscale()
    {
        this.svg.removeChild(this.g);
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(30, this.metrics.width, this.metrics.height, this.xMin, 
            this.xMax, this.yMin, this.yMax);
        this.wave = new Wave(this.signals, 0, this.g, this.metrics);
        this.grid = new Grid(this.g, this.metrics, this.labels)
        this.plot();
    }
    this.autoscale = autoscale;
    function getMinYInRange(x1, x2)
    {
        var ymin = this.signals[0].getMinYInRange(x1, x2);
        for(var sigIdx = 1; sigIdx < signals.length; sigIdx++)
        {
            var currMin = this.signals[sigIdx].getMinYInRange(x1, x2);
            if(currMin < ymin)
                ymin = currMin;
        }
        return ymin;
    }
    this.getMinYInRange = getMinYInRange;
    function getMaxYInRange(x1, x2)
    {
        var ymax = this.signals[0].getMaxYInRange(x1, x2);
        for(var sigIdx = 1; sigIdx < signals.length; sigIdx++)
        {
            var currMax = this.signals[sigIdx].getMaxYInRange(x1, x2);
            if(currMax > ymax)
                ymax = currMax;
        }
        return ymax;
    }
    this.getMaxYInRange = getMaxYInRange;
    
    function autoscaleY()
    {
        this.svg.removeChild(this.g);
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(30, this.metrics.width, this.metrics.height, this.metrics.xmin, 
            this.metrics.xmax, this.getMinYInRange(this.metrics.xmin,this.metrics.xmax),
            this.getMaxYInRange(this.metrics.xmin,this.metrics.xmax));
        this.wave = new Wave(this.signals, 0, this.g, this.metrics);
        this.grid = new Grid(this.g, this.metrics, this.labels)
        this.plot();
    }
    this.autoscaleY = autoscaleY;
    
    function setScale(xmin, xmax, ymin, ymax)
    {
        this.svg.removeChild(this.g);
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(30, this.metrics.width, this.metrics.height, xmin, xmax, ymin, ymax);
        this.wave = new Wave(this.signals, 0, this.g, this.metrics);
        this.grid = new Grid(this.g, this.metrics, this.labels)
        this.plot();
    }
    this.setScale = setScale;
    function setScaleAutoY(xmin, xmax)
    {
        this.svg.removeChild(this.g);
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(30, this.metrics.width, this.metrics.height, xmin, xmax, 
             this.getMinYInRange(xmin,xmax),
             this.getMaxYInRange(xmin,xmax));
        this.wave = new Wave(this.signals, 0, this.g, this.metrics);
        this.grid = new Grid(this.g, this.metrics, this.labels)
        this.plot();
    }
    this.setScaleAutoY = setScaleAutoY;
    
    function resetScales()
    {
        this.svg.removeChild(this.g);
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        var xmin, xmax, ymin, ymax;
        if(this.actXMin != undefined)
            xmin = this.actXMin;
        else
            xmin = this.xMin;
        if(this.actXMax != undefined)
            xmax = this.actXMax;
        else
            xmax = this.xMax;
        if(this.actYMin != undefined)
            ymin = this.actYMin;
        else
            ymin = this.yMin;
        if(this.actYMax != undefined)
            ymax = this.actYMax;
        else
            ymax = this.yMax;
        this.metrics = new Metrics(30, this.metrics.width, this.metrics.height, xmin, xmax, ymin, ymax);
        this.wave = new Wave(this.signals, 0, this.g, this.metrics);
        this.grid = new Grid(this.g, this.metrics, this.labels)
        this.plot();
    }
    this.resetScales = resetScales;

    
}


function getScopeTitle(tree, shot, title, success_cb,failure_cb,svg, scopeIdx, numCols, numRows, 
        col, row, labels) {
            
  var req = new XMLHttpRequest();
  var getStr;
  if (tree)
    getStr = '1darray/'+tree+'/'+shot+'?expr='+encodeUrl(title);
  else
    getStr = '1darray?expr='+encodeUrl(title);
  
  req.open('GET', getStr, true);
  req.scb=success_cb;
  req.fcb=failure_cb;
  req.svg=svg;
  req.scopeIdx = scopeIdx;
  req.numCols = numCols;
  req.numRows = numRows;
  req.col = col;
  req.row = row;
  req.labels = labels;
  req.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.getResponseHeader('ERROR'))
        this.fcb(this.tree,this.shot,this.expr,this.getResponseHeader('ERROR'),this.svg, this.scopeIdx, 
            this.numCols, this.numRows, this.col, this.row);
      else {
        this.scb(this.response, this.svg, this.scopeIdx, 
            this.numCols, this.numRows, this.col, this.row, this.labels);
      }
      //this.onreadystatechange = function() {};
    }
  };
  
  req.send();
}

function getScopeLimits(tree, shot, limit, limitId, success_cb,failure_cb,svg, scopeIdx, numCols, numRows, 
        col, row, labels) {
            
             
  var req = new XMLHttpRequest();
  var getStr;
  if (tree)
    getStr = '1darray/'+tree+'/'+shot+'?expr='+encodeUrl(limit);
  else
    getStr = '1darray?expr='+encodeUrl(limit);
  
  req.open('GET', getStr, true);
  req.scb=success_cb;
  req.fcb=failure_cb;
  req.svg=svg;
  req.scopeIdx = scopeIdx;
  req.numCols = numCols;
  req.numRows = numRows;
  req.col = col;
  req.row = row;
  req.labels = labels;
  req.limitId = limitId;
  req.responseType = 'arraybuffer';
  req.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.getResponseHeader('ERROR'))
       this.fcb(this.tree,this.shot,this.expr,this.getResponseHeader('ERROR'),this.svg, this.scopeIdx, 
            this.numCols, this.numRows, this.col, this.row);
      else {
        var limit;
        var dtype = this.getResponseHeader('DTYPE');
        //Convert in any case to an array
        if(dtype == 'Int8') dtype = 'Int8Array';
        if(dtype == 'Int16') dtype = 'Int16Array';
        if(dtype == 'Int32') dtype = 'Int32Array';
        if(dtype == 'Float32') dtype = 'Float32Array';
        if(dtype == 'Float64') dtype = 'Float64Array';
        var length = this.getResponseHeader('LENGTH');
 //       var data=eval('new '+dtype+'(this.response,0,length)');
        var data=eval('new '+dtype+'(this.response,0,1)');
        limit = data[0];
        limits = new Object();
        switch(this.limitId) {
            case LIMITS_XMIN: limits.xmin = limit; break;
            case LIMITS_XMAX: limits.xmax = limit; break;
            case LIMITS_YMIN: limits.ymin = limit; break;
            case LIMITS_YMAX: limits.ymax = limit; break;
        }  
        this.scb(this.svg, this.scopeIdx, 
            this.numCols, this.numRows, this.col, this.row, this.labels, limits);
      }
      //this.onreadystatechange = function() {};
    }
  };
  req.send();
}



function getScopeSignal(tree,shot,expr,color, mode, success_cb,failure_cb,svg, scopeIdx, numCols, numRows, 
        col, row, labels, limits) {
  var req = new XMLHttpRequest();
// var req = getXmlHttpRequestObject(); 
  req.responseType = 'arraybuffer';
  req.scb=success_cb;
  req.fcb=failure_cb;
  req.tree=tree;
  req.shot=shot;
/*  if(limits.xmin != undefined && limits.xmax != undefined) Possible management of segmented waeforms.......
  {
      req.expr = 'SetTimeContext('+limits.xmin+','+limits.xmax+',(('+limits.xmax+')-('+limits.xmin+'))/1000.);'+expr+';';
  }
  else
*/    req.expr=expr;
  req.color = color;
  req.mode = mode;
  req.svg=svg;
  req.scopeIdx = scopeIdx;
  req.numCols = numCols;
  req.numRows = numRows;
  req.col = col;
  req.row = row;
  req.labels = labels;
  
  //alert('Preparo richiesta per: '+ expr +' '+tree + ' '+shot);
  req.onreadystatechange = function() {
    if (req.readyState == 4) {
      var contType = req.getResponseHeader ("Content-Type");
      if (req.getResponseHeader('ERROR'))
      {
        alert("Error in get ScopeSignal: "+' '+req.expr);
        req.fcb(req.tree,req.shot,req.expr,req.getResponseHeader('ERROR'),req.svg, req.scopeIdx, 
            req.numCols, req.numRows, req.col, req.row);
      }
      else {
        var xdtype=req.getResponseHeader('XDTYPE');
        var ydtype=req.getResponseHeader('YDTYPE');
        var xlength=req.getResponseHeader('XLENGTH');
        var ylength=req.getResponseHeader('YLENGTH');
        var x=eval('new '+xdtype+'(this.response,0,xlength)');
        var y=eval('new '+ydtype+'(this.response,xlength*x.BYTES_PER_ELEMENT,ylength)');
        var tree=req.getResponseHeader('TREE');
        var shot=req.getResponseHeader('SHOT');
        this.scb(tree,shot,req.expr,x,y,req.color,req.mode,req.svg, req.scopeIdx, 
            req.numCols, req.numRows, req.col, req.row, req.labels);
        //alert("LETTO!!!"+xlength);
      }
  }
  else
  {
      //if (this.getResponseHeader('ERROR'))
      {
        //alert("ScopeSignal: "+' '+req.readyState+' '+req.expr);
      }
  }
  };




/*  req.onreadystatechange = function() {
    if (this.readyState == 4) {
      var contType = this.getResponseHeader ("Content-Type");
      if (this.getResponseHeader('ERROR'))
      {
        alert("Error in get ScopeSignal: "+' '+this.expr);
        this.fcb(this.tree,this.shot,this.expr,this.getResponseHeader('ERROR'),this.svg, this.scopeIdx, 
            this.numCols, this.numRows, this.col, this.row);
      }
      else {
        var xdtype=this.getResponseHeader('XDTYPE');
        var ydtype=this.getResponseHeader('YDTYPE');
        var xlength=this.getResponseHeader('XLENGTH');
        var ylength=this.getResponseHeader('YLENGTH');
        var x=eval('new '+xdtype+'(this.response,0,xlength)');
        var y=eval('new '+ydtype+'(this.response,xlength*x.BYTES_PER_ELEMENT,ylength)');
        var tree=this.getResponseHeader('TREE');
        var shot=this.getResponseHeader('SHOT');
        this.scb(tree,shot,this.expr,x,y,this.color,this.mode,this.svg, this.scopeIdx, 
            this.numCols, this.numRows, this.col, this.row, this.labels);
        //alert("LETTO!!!"+xlength);
      }
      //this.onreadystatechange = function() {};
      alert("ScopeSignal Received: "+this.readyState + ' '+this.expr);
  }
  else
  {
      //if (this.getResponseHeader('ERROR'))
      {
        alert("ScopeSignal: "+' '+this.readyState+' '+this.expr);
      }
  }
  };
*/
  //alert('GetScopeSignal for ' + req.expr);
  
 if (tree)
    req.open('POST','1dsignal/'+tree+'/'+shot+'?expr='+encodeUrl(expr),true);
  else
    req.open('POST','1dsignal?expr='+encodeUrl(expr),true);
  req.send();
}

var wavePanels = new Array();
var wavePanelsSvg = new Array();

var wavePanelCount = 0;


function mdsPlotFailure(tree,shot,expr,error,svg, scopeIdx, numCols, numRows, col, row) 
{
alert('ERRORE in '+expr+': '+error);
  if(wavePanelsSvg[scopeIdx] == undefined)
  {
      wavePanelCount++;
      var signals = new Array();
      var labels = new Object();
      labels.title = error;
      wavePanel = new WavePanel(signals,svg, numCols, numRows, col, row, labels);
      wavePanelsSvg[scopeIdx] = wavePanel;
      wavePanels.push(wavePanel);
  }
  else
  {
      wavePanel = wavePanelsSvg[scopeIdx];
      wavePanel.setTitle(error);
  }
  wavePanel.plot();
}


function scopeTitleSuccess(title, svg, scopeIdx, numCols, numRows, col, row, labels) 
{
  var wavePanel;
  if(wavePanelsSvg[scopeIdx] == undefined)
  {
      wavePanelCount++;
      var signals = new Array();
      labels.title = title;
      wavePanel = new WavePanel(signals,svg, numCols, numRows, col, row, labels);
      wavePanelsSvg[scopeIdx] = wavePanel;
      wavePanels.push(wavePanel);
  }
  else
  {
      wavePanel = wavePanelsSvg[scopeIdx];
      wavePanel.setTitle(title);
  }
  wavePanel.plot();
}

function scopeLimitsSuccess(svg, scopeIdx, numCols, numRows, col, row, labels, limits) 
{
    var wavePanel;
    if(wavePanelsSvg[scopeIdx] == undefined)
    {
        wavePanelCount++;
        var signals = new Array();
        wavePanel = new WavePanel(signals,svg, numCols, numRows, col, row, labels);
        wavePanelsSvg[scopeIdx] = wavePanel;
        wavePanel.setLimits(limits);
        wavePanels.push(wavePanel);
    }
    else
    {
        wavePanel = wavePanelsSvg[scopeIdx];
        wavePanel.setLimits(limits);
    }
    wavePanel.plot();
}

function scopePlotSuccess(tree,shot,expr,x,y,color, mode, svg, scopeIdx, numCols, numRows, col, row, labels) 
{
    
  var signals = new Array();
  signals.push(new Signal(x, y, color, mode));
  var wavePanel;
  if(wavePanelsSvg[scopeIdx] == undefined)
  {
      wavePanelCount++;
      wavePanel = new WavePanel(signals,svg, numCols, numRows, col, row, labels);
      wavePanelsSvg[scopeIdx] = wavePanel;
      wavePanels.push(wavePanel);
  }
  else
  {
      wavePanel = wavePanelsSvg[scopeIdx];
      wavePanel.pushSignal(new Signal(x,y,color, mode));
  }
  wavePanel.plot();
}

function resizeWaves(width, height)
{
    var idx;
    for(idx = 0; idx < wavePanels.length; idx++)
    {
        wavePanels[idx].resize(width, height);
    }
}


function resizeScope()
{
    var w=Math.round(window.innerWidth*0.9);
    var h=Math.round(window.innerHeight*0.9);
    resizeWaves(w,h);
}



var scopeIdx = 0;




function mdsScopePanel(div,width,height,numCols, numRows, col, row, tree,shot,exprArray, colors, modes, limits, labels) {
  var svg=document.createElementNS("http://www.w3.org/2000/svg","svg");
  
  //svg.setAttribute("ontouchstart", "alert('touchstart');");
    svg.addEventListener('touchstart', function(event) {
        touchStart(this, event);
    }, false);
    svg.addEventListener('touchmove', function(event) {
        touchMove(this, event);
    }, false);
    svg.addEventListener('touchend', function(event) {
        touchEnd(this, event);
    }, false);
    svg.setAttribute("version","1.1");
    svg.setAttributeNS(null,"preserveAspectRatio","none");
    svg.setAttribute("baseProfile","full");
    svg.setAttributeNS(null,"width",width);
    svg.setAttributeNS(null,"height",height);
    svg.setAttributeNS(null,"animatable","no");
    svg.setAttribute("onmousedown", "return mouseDown(this, event)");
    svg.setAttribute("onmousemove", "mouseMove(this, event)");
    svg.setAttribute("onmouseup", "return mouseUp(this, event)");
    //svg.setAttribute("onmouseclick", "return mouseClick(this, event)");
    svg.setAttribute("ontouchmove", "mouseMove(this, event)");
    svg.setAttribute("ontouchup", "mouseUp(this, event)");
    svg.setAttribute("viewBox","0 0 "+width +" " +height);
    div.appendChild(svg);


    var shotInt = parseInt(shot);
    if(shotInt >= -1 && tree != undefined)  //Not taking from global shots
    {
        for(var exprIdx = 0; exprIdx < exprArray.length; exprIdx++)
        {
            getScopeSignal(tree,shot,exprArray[exprIdx],colors[exprIdx], modes[exprIdx], scopePlotSuccess,mdsPlotFailure,svg, 
                scopeIdx, numCols, numRows, col, row, labels, limits);
        }
        if(labels.title != undefined)
        {
             getScopeTitle(tree, shot, labels.title, scopeTitleSuccess, mdsPlotFailure, svg, scopeIdx, 
                     numCols, numRows, col, row, labels);
        }
        if(limits.xmin != undefined)
        {
             getScopeLimits(tree, shot, limits.xmin, LIMITS_XMIN, scopeLimitsSuccess, mdsPlotFailure, svg, scopeIdx, 
                     numCols, numRows, col, row, labels);
        }
        if(limits.xmax != undefined)
        {
             getScopeLimits(tree, shot, limits.xmax, LIMITS_XMAX, scopeLimitsSuccess, mdsPlotFailure, svg, scopeIdx, 
                     numCols, numRows, col, row, labels);
        }
        if(limits.ymin != undefined)
        {
             getScopeLimits(tree, shot, limits.ymin, LIMITS_YMIN, scopeLimitsSuccess, mdsPlotFailure, svg, scopeIdx, 
                     numCols, numRows, col, row, labels);
        }
        if(limits.ymax != undefined)
        {
             getScopeLimits(tree, shot, limits.ymax, LIMITS_YMAX, scopeLimitsSuccess, mdsPlotFailure, svg, scopeIdx, 
                     numCols, numRows, col, row, labels);
        }
    }
    else //taking from global shots
    {
        //Add information to gloablShotPanels
        globalShotPanels.push({tree: tree, exprArray: exprArray, modes: modes, svg: svg, scopeIdx: scopeIdx, 
                numCols: numCols, numRows: numRows, col: col, row: row, limits: limits, labels: labels, 
                colors: colors, originalTitle: labels.title, limits: limits});
        if(globalShots.length > 0) //Global shots already defined
        {
            for(var shotIdx = 0; shotIdx < globalShots.length; shotIdx++)
            {
                for(var exprIdx = 0; exprIdx < exprArray.length; exprIdx++)
                {
                    getScopeSignal(tree,''+globalShots[shotIdx],exprArray[exprIdx],
                        colorPalette[(shotIdx * exprArray.length + exprIdx)%colorPalette.length], modes[exprIdx], 
                        scopePlotSuccess,mdsPlotFailure,svg, scopeIdx, numCols, numRows, col, row, labels, limits);
                }
            }
            if(labels.title != undefined)
            {
                 getScopeTitle(tree, globalShots[0], labels.title, scopeTitleSuccess, mdsPlotFailure, svg, scopeIdx, 
                         numCols, numRows, col, row, labels);
            }
            if(limits.xmin != undefined)
             {
                  getScopeLimits(tree, shot, limits.xmin, LIMITS_XMIN, scopeLimitsSuccess, mdsPlotFailure, svg, scopeIdx, 
                          numCols, numRows, col, row, labels);
             }
             if(limits.xmax != undefined)
             {
                  getScopeLimits(tree, shot, limits.xmax, LIMITS_XMAX, scopeLimitsSuccess, mdsPlotFailure, svg, scopeIdx, 
                          numCols, numRows, col, row, labels);
             }
             if(limits.ymin != undefined)
             {
                  getScopeLimits(tree, shot, limits.ymin, LIMITS_YMIN, scopeLimitsSuccess, mdsPlotFailure, svg, scopeIdx, 
                          numCols, numRows, col, row, labels);
             }
             if(limits.ymax != undefined)
             {
                  getScopeLimits(tree, shot, limits.ymax, LIMITS_YMAX, scopeLimitsSuccess, mdsPlotFailure, svg, scopeIdx, 
                          numCols, numRows, col, row, labels);
             }
        }
        else //Global shots not yet defined        
        {
            scopeTitleSuccess("Global Shot not yet defined", svg, scopeIdx, numCols, numRows, col, row, labels);
        }
    }       
    scopeIdx++;
}

function updateGlobalShots()
{
    var shotList = document.getElementById('GlobalShots').value;
    if(shotList == undefined)
        return;
    
    //First step: erase scopePanels
    
    var shots = shotList.trim().split(',');
    for(var shotIdx = 0; shotIdx < shots.length; shotIdx++)
    {
        var shot = parseInt(shots[shotIdx]);
        for(var panelIdx = 0; panelIdx < globalShotPanels.length; panelIdx++)
        {
            var wavePanel = wavePanels[globalShotPanels[panelIdx].scopeIdx];
            if(wavePanel != undefined)
                wavePanel.removeAllSignals();
            var exprArray = globalShotPanels[panelIdx].exprArray;
            for(var exprIdx = 0; exprIdx < exprArray.length; exprIdx++)
            {
                var color;
                if(shots.length == 1)
                    color = globalShotPanels[panelIdx].colors[exprIdx];
                else
                    color = colorPalette[(shotIdx * exprArray.length + exprIdx)%colorPalette.length];
                getScopeSignal(globalShotPanels[panelIdx].tree,shot,exprArray[exprIdx], color, 
                    globalShotPanels[panelIdx].modes[exprIdx], 
                    scopePlotSuccess,mdsPlotFailure,globalShotPanels[panelIdx].svg, globalShotPanels[panelIdx].scopeIdx,
                    globalShotPanels[panelIdx].numCols, globalShotPanels[panelIdx].numRows, globalShotPanels[panelIdx].col,
                    globalShotPanels[panelIdx].row, globalShotPanels[panelIdx].labels, globalShotPanels[panelIdx].limits);
            }
        }
    }
    if(shots.length > 0)
    {
        var shot = parseInt(shots[0]);
        for(var panelIdx = 0; panelIdx < globalShotPanels.length; panelIdx++)
        {
            var title;
            if(globalShotPanels[panelIdx].originalTitle != undefined)
                title = globalShotPanels[panelIdx].originalTitle;
            else
                title = '""';
            getScopeTitle(globalShotPanels[panelIdx].tree, shot, title, scopeTitleSuccess, 
                mdsPlotFailure, globalShotPanels[panelIdx].svg, globalShotPanels[panelIdx].scopeIdx, 
                globalShotPanels[panelIdx].numCols, globalShotPanels[panelIdx].numRows, globalShotPanels[panelIdx].col, 
                globalShotPanels[panelIdx].row, globalShotPanels[panelIdx].labels);
            

            if(globalShotPanels[panelIdx].limits.xmin != undefined)
            {
                  getScopeLimits(globalShotPanels[panelIdx].tree, shot, globalShotPanels[panelIdx].limits.xmin, LIMITS_XMIN, 
                        scopeLimitsSuccess, mdsPlotFailure, globalShotPanels[panelIdx].svg, 
                        globalShotPanels[panelIdx].scopeIdx, globalShotPanels[panelIdx].numCols, 
                        globalShotPanels[panelIdx].numRows, globalShotPanels[panelIdx].col, 
                        globalShotPanels[panelIdx].row, globalShotPanels[panelIdx].labels);
            }
            if(globalShotPanels[panelIdx].limits.xmax != undefined)
            {
                  getScopeLimits(globalShotPanels[panelIdx].tree, shot, globalShotPanels[panelIdx].limits.xmax, LIMITS_XMAX,
                        scopeLimitsSuccess, mdsPlotFailure, globalShotPanels[panelIdx].svg, 
                        globalShotPanels[panelIdx].scopeIdx, globalShotPanels[panelIdx].numCols, 
                        globalShotPanels[panelIdx].numRows, globalShotPanels[panelIdx].col, 
                        globalShotPanels[panelIdx].row, globalShotPanels[panelIdx].labels);
            }
            if(globalShotPanels[panelIdx].limits.ymin != undefined)
            {
                  getScopeLimits(globalShotPanels[panelIdx].tree, shot, globalShotPanels[panelIdx].limits.ymin, LIMITS_YMIN,
                        scopeLimitsSuccess, mdsPlotFailure, globalShotPanels[panelIdx].svg, 
                        globalShotPanels[panelIdx].scopeIdx, globalShotPanels[panelIdx].numCols, 
                        globalShotPanels[panelIdx].numRows, globalShotPanels[panelIdx].col, 
                        globalShotPanels[panelIdx].row, globalShotPanels[panelIdx].labels);
            }
            if(globalShotPanels[panelIdx].limits.ymax != undefined)
            {
                  getScopeLimits(globalShotPanels[panelIdx].tree, shot, globalShotPanels[panelIdx].limits.ymax, LIMITS_YMAX,
                        scopeLimitsSuccess, mdsPlotFailure, globalShotPanels[panelIdx].svg, 
                        globalShotPanels[panelIdx].scopeIdx, globalShotPanels[panelIdx].numCols, 
                        globalShotPanels[panelIdx].numRows, globalShotPanels[panelIdx].col, 
                        globalShotPanels[panelIdx].row, globalShotPanels[panelIdx].labels);
            }
        }
    }
}


function mdsScope(configXml)
{
    var parser;
      document.oncontextmenu = contextMenu;

    if (window.DOMParser)
    {
        parser=new DOMParser();
        xmlDoc=parser.parseFromString(configXml,"text/xml");
    }
    else // Internet Explorer
    {
        xmlDoc=new ActiveXObject("Microsoft.XMLDOM");
        xmlDoc.async=false;
        xmlDoc.loadXML(configXml);
    }
//Get Color palette
    var paletteXml = xmlDoc.getElementsByTagName('palette');
    if(paletteXml != undefined)
    {
        this.colorPalette = new Array();
        colorsXml = paletteXml[0].getElementsByTagName('color'); 
        for(var colorIdx = 0; colorIdx < colorsXml.length; colorIdx++)
            this.colorPalette.push(colorsXml[colorIdx].childNodes[0].nodeValue);
    }
    else
        this.colorPalette = ['black', 'red', 'blue', 'green'];

//Pass 1: build HTML table
    var htmlText = '<table><tr>';
    
    var columns = xmlDoc.getElementsByTagName('column');
    for(var colIdx = 0; colIdx < columns.length; colIdx++)
    {
        htmlText += '<td><table border = "1">';
        var rows = columns[colIdx].getElementsByTagName('panel');
        for(var rowIdx = 0; rowIdx < rows.length; rowIdx++)
        {
            htmlText += '<tr><td style="height=100%" id="scope_'+rowIdx+'_'+colIdx+'">'+'</td></tr>';
        }
        htmlText += '</table></td>';
    }
    htmlText += '</tr></table>'+bottomLine;
    document.getElementById("ScopeArea").innerHTML=htmlText;
    //alert(htmlText);
    //Pass 2: create scope panels
    for(var colIdx = 0; colIdx < columns.length; colIdx++)
    {
        var panels = columns[colIdx].getElementsByTagName('panel');
        for(var panelIdx = 0; panelIdx < panels.length; panelIdx++)
        {
            var experiment = panels[panelIdx].getAttribute("tree");
            var shot = panels[panelIdx].getAttribute("shot");
            var signals = panels[panelIdx].getElementsByTagName("signal");
            var colors = new Array();
            var modes = new Array();
            var expressions = new Array();
            var limits = new Object();
            if(panels[panelIdx].hasAttribute("xmin"))
                limits.xmin = panels[panelIdx].getAttribute("xmin");
            if(panels[panelIdx].hasAttribute("xmax"))
                limits.xmax = panels[panelIdx].getAttribute("xmax");
            if(panels[panelIdx].hasAttribute("ymin"))
                limits.ymin = panels[panelIdx].getAttribute("ymin");
            if(panels[panelIdx].hasAttribute("ymax"))
                limits.ymax = panels[panelIdx].getAttribute("ymax");
            var labels = new Object();
            {
                if(panels[panelIdx].hasAttribute("title"))
                    labels.title = panels[panelIdx].getAttribute("title")
                if(panels[panelIdx].hasAttribute("xlabel"))
                    labels.xlabel = panels[panelIdx].getAttribute("xlabel")
                if(panels[panelIdx].hasAttribute("ylabel"))
                    labels.ylabel = panels[panelIdx].getAttribute("ylabel")
            }
            for(var signalIdx = 0; signalIdx < signals.length; signalIdx++)
            {
                var color;
                if(signals[signalIdx].hasAttribute("color"))
                    color = parseInt(signals[signalIdx].getAttribute("color"));
                else
                    color = 0;
                colors.push(this.colorPalette[color % this.colorPalette.length]);
                var mode;
                if(signals[signalIdx].hasAttribute("mode"))
                    mode = parseInt(signals[signalIdx].getAttribute("mode"));
                else
                    mode = PLOT_LINE;
                modes.push(mode);
                var expression = signals[signalIdx].childNodes[0].nodeValue;
                expressions.push(expression);
            }
            mdsScopePanel(document.getElementById('scope_'+panelIdx+'_'+colIdx),Math.round(window.innerWidth * 0.9/columns.length),
                    Math.round(window.innerHeight * 0.9/panels.length),columns.length, panels.length, colIdx, panelIdx, experiment,shot,
                    expressions, colors, modes, limits, labels);
    
        }
    }
    
}

function startMdsScope(configName)
{
    var req = new XMLHttpRequest();
    req.open('POST','?configxml='+configName,true);
    req.onreadystatechange = function() {
        if (this.readyState == 4) {
            if (this.getResponseHeader('ERROR'))
                alert('Error getting Scope configuration');
            else {
                alert(this.response);
                var isConfig = this.getResponseHeader('IS_CONFIG');
                if(isConfig == 'YES')
                    mdsScope(this.response);
                else
                    document.getElementById("ScopeArea").innerHTML = this.response;
            }
            //this.onreadystatechange = function() {};
        }
    };
    req.send();
}
