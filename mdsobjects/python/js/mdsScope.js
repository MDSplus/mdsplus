var mouseTarget;
var ZOOM = 1;
var CROSSHAIR = 2;
var PAN = 3;
var mode = ZOOM;
//Double click management
var alreadyClicked = false;
var alreadyDoubleClicked = false;

var stopRightMouse=false;
var colorPalette;
var contextMenuSvg; //svg above which righ button has been clicked
var globalShotPanels = new Array(); //Array containing information required for wave panels taking global shot number
var globalShots = new Array(); //Array containing global shot numbers (as defined in the bottom form)
var wavePanels = new Array();

//Limit costants used for  limits management
var LIMITS_XMIN = 1;
var LIMITS_XMAX = 2;
var LIMITS_YMIN = 3;
var LIMITS_YMAX = 4;

var METRICS_BORDER = 2;
var mouseMovingPanel; //The WaevPanel into which the mouse is being moved

var bottomLine = 
    '<form name="scopeMode" >'+
    '<table cellpadding="20" cellspacing="0" ><tr><td><label for="Zoom"><input type="radio" checked = "yes" name="Mode" id="Zoom" onclick="setMode(ZOOM);"/>Zoom</label>'+
    '<label for="Crosshair"> <input type="radio" name="Mode" id="Crosshair" onclick="setMode(CROSSHAIR);"/>Crosshair</label>'+
    '<label for="Pan"><input type="radio" name="Mode" id="Pan" onclick="setMode(PAN);"/> Pan</label></td><td>'+
    '<label for="GlobalShots">Shots</label><input type = "text" id="GlobalShots"  onkeydown="if (event.keyCode == 13) {event.preventDefault(); updateGlobalShots();}"/>'+
    '<button type="button"  autofocus id = "UpdateGlobal" onclick="updateGlobalShots()">Update</button></td><td><label id="XValue" /></td><td><label id="YValue" /></td><td>'+
    '<label id="Expr" /></td><td align = "top"><label id="Shot" /></td></tr></table>'+
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

function contextMenu(e)
{
    if (stopRightMouse) {
      stopRightMouse=false;
      e.stopPropagation();
      return false;
    } else
      return true;
}

function fixEvent(e) {
  if (! e.hasOwnProperty('offsetX')) {
    var curleft = curtop = 0;
    if (e.offsetParent) {
      var obj=e;
      do {
        curleft += obj.offsetLeft;
        curtop += obj.offsetTop;
      } while (obj = obj.offsetParent);
    } 
    e.offsetX=e.layerX-curleft;
    e.offsetY=e.layerY-curtop;
  }
  return e;
}

function anyZooming()
{
    for(idx = 0; idx < wavePanels.length; idx++)
    {
        if(wavePanels[idx].isZooming())
            return true;
    }
    return false;
}
 
function mouseDown(e)
{
    var idx;
    if (e == null)
      e=window.event;
    e.preventDefault();
    e.stopPropagation();
    fixEvent(e);
    var popupMenu = document.getElementById("ScopePopup"); 
    popupMenu.style.display = 'none';
    e.preventDefault();

    if(e.button == 2) //MB3 (right) button pressed
    {
        if(anyZooming())
            return false;
        stopRightMouse=true;
        contextMenuSvg = e.target;
        var popupMenu = document.getElementById("ScopePopup"); 
        popupMenu.style.display = 'inherit';
        var left=e.pageX-120;
        var top=e.pageY-70;
        if (left < 0)
          left=0;
        if (top < 0)
          top=0;
        popupMenu.style.left = left+'px';
        popupMenu.style.top = top + 'px';
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
        for(idx = 0; idx < wavePanels.length; idx++)
        {
            if(wavePanels[idx].svg == e.target)
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
        if(e.target.localName == 'text')
        {
            downX = e.target.offsetLeft + e.offsetX;
            downY = e.target.offsetTop + e.offsetY;
        }
        else
        {
            downX = e.offsetX;
            downY = e.offsetY;
        }
//        if(wavePanels[idx].svg == e.target)
        if(wavePanels[idx].svg == e.currentTarget)
        {
            mouseMovingPanel = wavePanels[idx];
            e=fixEvent(e);
            switch(mode) {
                case ZOOM: 
                    wavePanels[idx].enableMouseMove();
                    startZoomX = downX;
                    startZoomY = downY;
                    wavePanels[idx].startZoom(downX, downY);
                    break;
                case CROSSHAIR:
                    wavePanels[idx].enableMouseMove();
                    for(var currIdx = 0; currIdx < wavePanels.length; currIdx++)
                    {
                        if(currIdx != idx)
                            wavePanels[currIdx].endCrosshair();
                    }
                    wavePanels[idx].startCrosshair(downX, downY);
                    break;
                case PAN:
                    wavePanels[idx].enableMouseMove();
                    wavePanels[idx].startPan(downX, downY);
                    break;

            }
         }
    }
    return true;
}


function mouseMove(e)
{
    var idx;
    if ( e == null )
      e=window.event;
    e.preventDefault();
    fixEvent(e);
 //   if(mouseTarget != e.target)
 //       return;
    if(e.button == 2) //MB3 (right) button pressed
    {
        if(anyZooming())
            return;
    }
    if(mouseMovingPanel != undefined)
    {
        if(mouseMovingPanel.svg == e.currentTarget)
        {
            e=fixEvent(e);
            var moveX, moveY;
            if(e.target.localName == 'text')
            {
                moveX = e.target.offsetLeft + e.offsetX;
                moveY = e.target.offsetTop + e.offsetY;
            }
            else
            {
                moveX = e.offsetX;
                moveY = e.offsetY;
            }
            switch(mode) {
                case ZOOM: 
                    // Used in case mouse button is released in a different target 
                    mouseMovingPanel.dragZoom(moveX, moveY);
                    break;
                case CROSSHAIR:
                    mouseMovingPanel.dragCrosshair(moveX, moveY);
                    break;
                case PAN:
                    mouseMovingPanel.dragPan(moveX, moveY);
                    break;
            }
        }
    }
}
function mouseUp(e)
{
    var idx;
    if (e == null)
      e=window.event;
 //   e.preventDefault();
    fixEvent(e);
    var upX, upY;
    if(e.target.localName == 'text')
    {
        upX = e.target.offsetLeft + e.offsetX;
        upY = e.target.offsetTop + e.offsetY;
    }
    else
    {
        upX = e.offsetX;
        upY = e.offsetY;
    }

    //Handle the possibility that the mouse button is released on another panel
    if(mode == ZOOM)
    {
        mouseMovingPanel.disableMouseMove();
        if(mouseMovingPanel.isZooming())
        {
            e=fixEvent(e);
            mouseMovingPanel.endZoom(upX, upY);
        }
    }
    else
    {
        for(idx = 0; idx < wavePanels.length; idx++)
        {
            e=fixEvent(e);
            switch(mode) {
                case CROSSHAIR:
                    wavePanels[idx].disableMouseMove();
                    wavePanels[idx].endCrosshair();
                    break;
                case PAN:
                    wavePanels[idx].disableMouseMove();
                    wavePanels[idx].endPan();
                    break;
                }
        }
    }
}

//Touch event management 
var touchMoved;
function touchStart(e)
{
    if (e == null)
      e=window.event;
    var idx;
    contextMenuSvg = e.target;
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
        
        if(wavePanels[idx].svg == e.target)
        {
            var touchX = e.touches[0].clientX - wavePanels[idx].col*e.target.clientWidth;
            var touchY = e.touches[0].clientY - wavePanels[idx].row*e.target.clientHeight;
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
function touchMove(e)
{
    if (e == null) 
      e=window.event;
    var idx;
    e.preventDefault();
    touchMoved = true;
    var node = e.target;
    for(idx = 0; idx < wavePanels.length; idx++)
    {
        if(wavePanels[idx].svg == e.target)
        {
            touchMoveX = e.touches[0].clientX - wavePanels[idx].col*e.target.clientWidth;
            touchMoveY = e.touches[0].clientY- wavePanels[idx].row*e.target.clientHeight;
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
function touchEnd(e)
{
    if (e == null)
      e=window.event;
    var idx;
    e.preventDefault();
    for(idx = 0; idx < wavePanels.length; idx++)
    {
        if(wavePanels[idx].svg == e.target)
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

function replaceAll(source,stringToFind,stringToReplace)
{
    var temp = source;
    var index = temp.indexOf(stringToFind);
    while(index != -1)
    {
        temp = temp.replace(stringToFind,stringToReplace);
        index = temp.indexOf(stringToFind);
    }
    return temp;
}

function decodeUrl(inStr)
{
    var urlStr = replaceAll(inStr, "%25", "%"); 
    urlStr = replaceAll(urlStr, "%2F", "/"); 
    urlStr = replaceAll(urlStr, "%23", "#"); 
    urlStr = replaceAll(urlStr, "%3F", "?"); 
    urlStr = replaceAll(urlStr, "%26", "&"); 
    urlStr = replaceAll(urlStr, "%2C", ","); 
    urlStr = replaceAll(urlStr, "%3D", "="); 
    urlStr = replaceAll(urlStr, "%40", "@"); 
    urlStr = replaceAll(urlStr, "%3B", ";"); 
    urlStr = replaceAll(urlStr, "%24", "$"); 
    urlStr = replaceAll(urlStr, "%2B", "+"); 
    urlStr = replaceAll(urlStr, "%20", " ");
    urlStr = replaceAll(urlStr, "%3E", ">");
    urlStr = replaceAll(urlStr, "%3C", "<");
    urlStr = replaceAll(urlStr, "%3A", ":");
    urlStr = replaceAll(urlStr, "%5B", "[");
    return replaceAll(urlStr, "%5D", "]");
}





 //////////////////////////////////////////////////////////////////////////
/**
*  Class Signal conytains Signal-related information including color and marker mode
**/
var PLOT_LINE = 1;
var PLOT_POINT = 2;
var PLOT_LINE_POINT = 3;

function Signal(x, y, color, mode, expr, shot)
{
    this.x = x;
    this.y = y;
    this.expr = expr;
    this.shot = shot;
    this.size = x.length;
    if(this.size > y.length)
        this.size = y.length;
    if(color == undefined)
        this.color = 'black';
    else
        this.color = color;
    if(this.color == 'MediumSlate')  //Supported by X11 but not by SVG
        this.color = 'blue';
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
                xmin = this.x[idx]+0.;
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
                xmax = this.x[idx]+0.;
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
                ymin = this.y[idx]+0.;
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
                ymax = this.y[idx]+0.;
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
                ymin = this.y[idx];
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
                ymax = this.y[idx];
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
    
   //if no limuts passed provide default limits
    if (this.xmin == undefined && this.xmax == undefined)
    {
        this.xmin = 0.; 
        this.xmax = 1;    
    } 
    else
    {
         if(this.xmax == undefined)
            this.xmax = this.xmin + 1.;
        if(this.xmin == undefined)
            this.xmin = this.xmax - 1.;
    }
    if (this.ymin == undefined && this.ymax == undefined)
    {
        this.ymin = 0.; 
        this.ymax = 1;    
    } 
    else
    {
        if(this.ymax == undefined)
            this.ymax = this.ymin + 1.;
        if(this.ymin == undefined)
            this.ymin = this.ymax - 1.;
    }
    if(this.ymin >= this.ymax)
        this.ymax = this.ymin + 1E-3;
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
        if(isNaN(yPixel))
          return undefined;
        else
	  return yPixel;
    }
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
    this.xmin = metrics.xmin;
    this.xmax = metrics.xmax;
    this.ymin = metrics.ymin;
    this.ymax = metrics.ymax;
    this.width = metrics.width;
    this.height = metrics.height;
    this.metrics = metrics;
    this.labels = labels;
    this.g = g;
    
    function buildGrid(minValue, maxValue, numSteps)
    {
        var i;
        if (maxValue < minValue)
            maxValue = minValue + 1E-10;

        var range = maxValue - minValue;
        if (range <= 0)
            range = 1E-3;

        var currMaxValue = maxValue;// + 0.1 * range;
        var currMinValue = minValue;// - 0.1 * range;
        var step = (maxValue - minValue) / numSteps;

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
    this.xVals = this.buildGrid(this.xmin, this.xmax, 8);
    this.yVals = this.buildGrid(this.ymin, this.ymax, 5);
    
    function roundLabel(label)
    {
 //       var dotPos = label.indexOf(".");
 //       if(dotPos == -1 || (label.length - dotPos) < 8)
 //           return label;
        var rounded;
        if(Math.abs(label) < 1E10)
             rounded = Math.round(parseFloat(label)*100000)/100000;
        else
             rounded = Math.round(parseFloat(label)*10)/10;
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
            fontSize = this.height / this.yVals.length;
            if(fontSize > 16)
                fontSize = 16;
            if(fontSize < 2)
                fontSize = 2;
            label.setAttributeNS(null,"font-size",fontSize+"px");
//            label.setAttributeNS(null,"font-size","16px");
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
            title.setAttributeNS(null,"font-size","14px");
            /*if(this.labels.title.length < 20)
             else if(this.labels.title.length < 40)
             title.setAttributeNS(null,"font-size","12px");
            else
             title.setAttributeNS(null,"font-size","8px"); */
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


function Wave(signals, color, g, metrics, clippath)
{
    this.signals = signals;
    this.g = g;
    this.color = color;
    this.metrics = metrics;
    this.clippath = clippath;
    function plot()
    {
        var idx;
        var signalIdx;
        var clip_path_id=this.clippath.getAttribute("id");
        this.g.setAttribute("clip-path","url(#"+clip_path_id+")");
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
                var lastYs;
                var firstYs = minYs;
                for (idx=0; idx < this.signals[signalIdx].size; idx++) {
                    var xs=this.metrics.getXPixel(this.signals[signalIdx].x[idx]);
                    var ys=this.metrics.getYPixel(this.signals[signalIdx].y[idx]);
		    if(ys == undefined || xs == undefined) continue;
		    if(idx > 0 && xs == prevXs)
		    {
			if(ys < minYs) minYs = ys;
			if(ys > maxYs) maxYs = ys;
                        lastYs = ys;
		    }
		    else
		    {
		        if(minYs == undefined)
			{
			    minYs = ys;
			    maxYs = ys;
			}
                        if(firstYs != minYs)
                        {
                               ans = ans + ' ' + prevXs + ' ' +firstYs;
                        }
			ans = ans + ' ' + prevXs + ' ' +minYs;
			if (maxYs > minYs)
			{
 			   ans = ans + ' ' + prevXs + ' ' +maxYs;
                           if(lastYs != maxYs)
                           {
                               ans = ans + ' ' + prevXs + ' ' +lastYs;
                           }
			}
			minYs = ys;
			maxYs = ys;
                        firstYs = ys;
                        lastYs = ys;
			prevXs = xs;
                        
		    } 

                    //Draw last point 
                    if(idx == this.signals[signalIdx].size - 1)
                    {
                        ans = ans + ' ' + xs + ' ' +ys;
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
        if(this.signals == undefined || sigIdx >= this.signals.length)
            return "Black";
        return this.signals[sigIdx].color;
    }
    this.getCrosshairColor = getCrosshairColor;
    function getCrosshairExpr(sigIdx)
    {
        return signals[sigIdx].expr;
    }
    this.getCrosshairExpr = getCrosshairExpr;
    function getCrosshairShot(sigIdx)
    {
        return signals[sigIdx].shot;
    }
    this.getCrosshairShot = getCrosshairShot;
    
}    
 
 function WavePanel(svg, panelIdx, numCols, numRows, col, row, clippath, tree, shots, colors, modes, signalExprs, limitExprs, labelExprs, defaultNodeExpr)
{
    this.STARTING_ZOOM = 1;
    this.ZOOMING = 2;
    this.NOT_ZOOMING = 3;
    this.zoomState = this.NOT_ZOOMING;
    this.svg = svg;
    this.numCols = numCols;
    this.numRows = numRows;
    this.col = col;
    this.row = row;
    this.panelIdx = panelIdx;
    this.clippath = clippath;

    this.borderRect=document.createElementNS("http://www.w3.org/2000/svg","rect");
    this.borderRect.setAttribute("x","0");
    this.borderRect.setAttribute("y","0");
    this.borderRect.setAttribute("width",svg.viewBox.baseVal.width);
    this.borderRect.setAttribute("height",svg.viewBox.baseVal.height);
    this.borderRect.setAttribute("fill","none");
    this.borderRect.setAttribute("stroke","black");
    svg.appendChild(this.borderRect);
  
  //Configuration definition
    this.tree = tree;
    this.shots = shots;
    this.colors = colors;
    this.modes = modes;
    this.signalExprs = signalExprs;
    this.limitExprs = limitExprs;
    this.labelExprs = labelExprs;
    this.defaultNodeExpr = defaultNodeExpr;

    function enableMouseMove()
    {
        this.svg.addEventListener('mousemove', this.mouseMove = function(evt) {
            mouseMove(evt);
        }, false);
        //var childNodes = this.g.childNodes;
        //for(var i = 0; i < childNodes.length; i++)
        //    childNodes[i].addEventListener('mousemove', this.mouseMove = function(evt) {
        //        }, true);
    }
    this.enableMouseMove = enableMouseMove;
    
    function disableMouseMove()
    {
        this.g.removeEventListener('mousemove', this.mouseMove, false);
    }
    this.disableMouseMove = disableMouseMove;
     
    function update()
    {
        var req = new XMLHttpRequest();
        var getStr = 'scope?panel=yes&y1='+this.signalExprs[0];
        for(var i = 1; i < signalExprs.length; i++)
        {
            getStr = getStr + '&y'+(i+1)+'='+this.signalExprs[i];
        }
        if (this.tree != undefined) {
            getStr = getStr+'&tree='+this.tree+'&';
            if(this.shots != undefined)
            {
              if(this.shots.length > 0)
              {
                getStr = getStr+'shot='+this.shots[0];
                for(var i = 1; i < this.shots.length; i++)
                {
                    getStr = getStr + ','+this.shots[i];
                }
              }
            }
        }
        if(this.labelExprs.title != undefined)
             getStr = getStr + '&title='+labelExprs.title;
        if(this.labelExprs.xLabel != undefined)
             getStr = getStr + '&xlabel='+labelExprs.xLabel;
        if(this.labelExprs.yLabel != undefined)
             getStr = getStr + '&ylabel='+labelExprs.yLabel;
        if(this.limitExprs.xMin != undefined)
             getStr = getStr + '&xmin='+this.limitExprs.xMin;
        if(this.limitExprs.xMax != undefined)
             getStr = getStr + '&xmax='+this.limitExprs.xMax;
        if(this.limitExprs.yMin != undefined)
             getStr = getStr + '&ymin='+this.limitExprs.yMin;
        if(this.limitExprs.yMax != undefined)
             getStr = getStr + '&ymax='+this.limitExprs.yMax;

//alert(getStr);
        req.open('GET', getStr, true);
//        req.open('POST', getStr, true);
        req.col = this.col;
        req.row = this.row;
        req.colors = this.colors;
        req.modes = this.modes;
        req.numSignals = this.signalExprs.length;
        req.limitExprs = this.limitExprs;
        req.labelExprs = this.labelExprs;
        req.panelIdx = this.panelIdx;
        req.shots = this.shots;
        req.signalExprs = this.signalExprs;
        if(this.shots == undefined)
            req.numShots = 0;
        else
            req.numShots = this.shots.length;
        req.onreadystatechange = function() {
            if (req.readyState == 4) 
            {
                if (this.getResponseHeader('ERROR'))
                    updateFailure(this.col, this.row, this.getResponseHeader('ERROR'));
                else
                {
                    var signals = new Array();
                    var errors = '';
                    if(this.numShots <= 1)
                    {
                        var currOffset = 0;
                        for(var sigIdx = 0; sigIdx < this.numSignals; sigIdx++)
                        {
                            var currError = this.getResponseHeader('ERROR'+(sigIdx + 1));
                            if(currError != undefined)
                                errors = errors + '\n'+currError;
                            else
                            {
                                var xdtype=this.getResponseHeader('X'+(sigIdx + 1)+'_DATATYPE');
                                var ydtype=this.getResponseHeader('Y'+(sigIdx + 1)+'_DATATYPE');
                                var xlength=parseInt(this.getResponseHeader('X'+(sigIdx + 1)+'_LENGTH'));
                                var ylength=parseInt(this.getResponseHeader('Y'+(sigIdx + 1)+'_LENGTH'));
                                var x=eval('new '+xdtype+'(this.response,currOffset,xlength)');
                                var y=eval('new '+ydtype+'(this.response,currOffset + xlength*x.BYTES_PER_ELEMENT,ylength)');
                                currOffset = currOffset + xlength*x.BYTES_PER_ELEMENT+ylength*y.BYTES_PER_ELEMENT;
                                var shotStr = '';
                                if(this.numShots == 1)
                                    shotStr = ''+this.shots[0];
                                signals.push(new Signal(x, y, this.colors[sigIdx], this.modes[sigIdx], decodeUrl(this.signalExprs[sigIdx]), shotStr));
                            }
                        }
                    }  
                    else
                    {
                        var currSigIdx = 0;
                        var currOffset = 0;
                        for(var shotIdx = 0; shotIdx < this.numShots; shotIdx++)
                        {
                            for(var sigIdx = 0; sigIdx < this.numSignals; sigIdx++)
                            {
                                var currError = this.getResponseHeader('ERROR'+(currSigIdx + 1));
                                if(currError != undefined)
                                    errors = errors + '\n'+currError;
                                else
                                {
                                    var xdtype=this.getResponseHeader('X'+(currSigIdx + 1)+'_DATATYPE');
                                    var ydtype=this.getResponseHeader('Y'+(currSigIdx + 1)+'_DATATYPE');
                                    var xlength=parseInt(this.getResponseHeader('X'+(currSigIdx + 1)+'_LENGTH'));
                                    var ylength=parseInt(this.getResponseHeader('Y'+(currSigIdx + 1)+'_LENGTH'));
                                    var x=eval('new '+xdtype+'(this.response,currOffset,xlength)');
                                    var y=eval('new '+ydtype+'(this.response,currOffset + xlength*x.BYTES_PER_ELEMENT,ylength)');
                                    currOffset = currOffset + xlength*x.BYTES_PER_ELEMENT+ylength*y.BYTES_PER_ELEMENT;
                                    var shotStr = ''+this.shots[shotIdx];
                                    signals.push(new Signal(x, y, colorPalette[currSigIdx%colorPalette.length], this.modes[sigIdx], decodeUrl(this.signalExprs[sigIdx]), shotStr));
                                }
                                currSigIdx++;
                            }
                        }
                    }
                    var limits = new Object();
                    if(this.limitExprs.xMin != undefined)
                        limits.xmin = parseFloat(this.getResponseHeader('xmin'));
                    if(this.limitExprs.xMax != undefined)
                        limits.xmax = parseFloat(this.getResponseHeader('xmax'));
                    if(this.limitExprs.yMin != undefined)
                        limits.ymin = parseFloat(this.getResponseHeader('ymin'));
                    if(this.limitExprs.yMax != undefined)
                        limits.ymax = parseFloat(this.getResponseHeader('ymax'));
                    
                    var labels = new Object();
                    if(this.labelExprs.title != undefined)
                        labels.title = this.getResponseHeader('title') + errors;
                    else 
                    {
                        if(errors != '')
                            labels.title = errors;
                    }    
                    if(this.labelExprs.xLabel != undefined)
                        labels.xlabel = this.getResponseHeader('xlabel');
                    if(this.labelExprs.yLabel != undefined)
                        labels.ylabel = this.getResponseHeader('ylabel');
                    
                    updateSuccess(this.panelIdx, signals, labels, limits);
                }
            }
        }
        req.responseType='arraybuffer';
        req.send();
    }
    this.update = update;
    
    function plot()
    {
        this.grid.plot();
        this.wave.plot();
    }
    this.plot = plot;

    function setPanel(signals, labels, limits)
    {
    
        if(this.g != undefined)
            this.svg.removeChild(this.g);
        this.zoomRect = undefined;
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.svg.appendChild(this.g);

        this.signals = signals;
        this.limits = limits;
        this.labels = labels;
    //default limits when no signal
        this.xMin = 0.;
        this.xMax = 1.;
        this.yMin = 0.;
        this.yMax = 1.;
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
   //Override default limits with given ones
        if(this.limits.xmin != undefined)
            this.xMin = this.limits.xmin;
        if(this.limits.xmax != undefined)
            this.xMax = this.limits.xmax;
        if(this.limits.ymin != undefined)
            this.yMin = this.limits.ymin;
        if(this.limits.ymax != undefined)
            this.yMax = this.limits.ymax;
 
        var metrics = new Metrics(METRICS_BORDER, this.svg.viewBox.baseVal.width, this.svg.viewBox.baseVal.height, 
                this.xMin, this.xMax, this.yMin, this.yMax);
        var wave = new Wave(this.signals, 0, this.g, metrics, this.clippath);
        var grid = new Grid(this.g, metrics, this.labels);
        this.metrics = metrics;
        this.wave = wave;
        this.grid = grid;
        this.plot();
    }
    this.setPanel = setPanel;

    if(shots != undefined && shots[0] < -1) //No global shot defined yet
    {
        var currLabels = new Array();
        currLabels.title = 'Global shot not yet defined';
        this.setPanel(new Array(), currLabels, new Object()); 
    }
    else
        this.update();
    

    function removeAllSignals()
    {
        this.signals = new Array();
        this.svg.removeChild(this.g);
        this.zoomRect = undefined;
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.svg.appendChild(this.g);
        this.xMin = undefined;
        this.xMax = undefined;
        this.yMin = undefined;
        this.yMax = undefined;
        var metrics = new Metrics(METRICS_BORDER, svg.viewBox.baseVal.width, svg.viewBox.baseVal.height, 
            0., 1. ,0., 1.);
        var wave = new Wave(this.signals, 0, this.g, metrics, this.clippath);
        var grid = new Grid(this.g, metrics, this.labels);
        this.metrics = metrics;
        this.wave = wave;
        this.grid = grid;
        this.plot();
    }
    this.removeAllSignals = removeAllSignals;

 
    
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
	this.zoomRect = undefined;
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(METRICS_BORDER, width/this.numCols, height/this.numRows, this.metrics.xmin, 
                this.metrics.xmax, this.metrics.ymin, this.metrics.ymax);
        this.wave = new Wave(this.signals, this.colors, this.g, this.metrics, this.clippath);
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
        document.getElementById("XValue").innerHTML = 'X:'+(Math.round(xVal * 100000)/100000);
        document.getElementById("YValue").innerHTML ='Y:'+Math.round(yVal * 100000)/100000;
        document.getElementById("Expr").innerHTML ='Expr:'+this.wave.getCrosshairExpr(this.crosshairIdx);
        document.getElementById("Shot").innerHTML ='Shot:'+this.wave.getCrosshairShot(this.crosshairIdx);
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
        this.metrics = new Metrics(METRICS_BORDER, this.originalPanMetrics.width, this.originalPanMetrics.height, 
          this.originalPanMetrics.xmin + deltaX, this.originalPanMetrics.xmax + deltaX, 
          this.originalPanMetrics.ymin + deltaY,  this.originalPanMetrics.ymax + deltaY);
        this.svg.removeChild(this.g);
	this.zoomRect = undefined;
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.wave = new Wave(this.wave.signals, 0, this.g, this.metrics, this.clippath);
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
	this.zoomRect = undefined;
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
    function isZooming()
    {
        return (this.zoomState == this.ZOOMING);
    }
    this.isZooming = isZooming;
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
	this.zoomRect = undefined;
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(METRICS_BORDER, this.metrics.width, this.metrics.height, xMin, xMax, yMin, yMax);
        this.wave = new Wave(this.signals, 0, this.g, this.metrics, this.clippath);
        this.grid = new Grid(this.g, this.metrics, this.labels)
        this.plot();
    }
    this.endZoom = endZoom;
    function autoscale()
    {
        this.svg.removeChild(this.g);
	this.zoomRect = undefined;
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(METRICS_BORDER, this.metrics.width, this.metrics.height, this.xMin, 
            this.xMax, this.yMin, this.yMax);
        this.wave = new Wave(this.signals, 0, this.g, this.metrics, this.clippath);
        this.grid = new Grid(this.g, this.metrics, this.labels)
        this.plot();
    }
    this.autoscale = autoscale;
    function getMinYInRange(x1, x2)
    {
        var ymin = this.signals[0].getMinYInRange(x1, x2);
        for(var sigIdx = 1; sigIdx < this.signals.length; sigIdx++)
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
        for(var sigIdx = 1; sigIdx < this.signals.length; sigIdx++)
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
	this.zoomRect = undefined;
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(METRICS_BORDER, this.metrics.width, this.metrics.height, this.metrics.xmin, 
            this.metrics.xmax, this.getMinYInRange(this.metrics.xmin,this.metrics.xmax),
            this.getMaxYInRange(this.metrics.xmin,this.metrics.xmax));
        this.wave = new Wave(this.signals, 0, this.g, this.metrics, this.clippath);
        this.grid = new Grid(this.g, this.metrics, this.labels)
        this.plot();
    }
    this.autoscaleY = autoscaleY;
    
    function setScale(xmin, xmax, ymin, ymax)
    {
        this.svg.removeChild(this.g);
	this.zoomRect = undefined;
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(METRICS_BORDER, this.metrics.width, this.metrics.height, xmin, xmax, ymin, ymax);
        this.wave = new Wave(this.signals, 0, this.g, this.metrics, this.clippath);
        this.grid = new Grid(this.g, this.metrics, this.labels)
        this.plot();
    }
    this.setScale = setScale;
    function setScaleAutoY(xmin, xmax)
    {
        this.svg.removeChild(this.g);
	this.zoomRect = undefined;
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        this.metrics = new Metrics(METRICS_BORDER, this.metrics.width, this.metrics.height, xmin, xmax, 
             this.getMinYInRange(xmin,xmax),
             this.getMaxYInRange(xmin,xmax));
        this.wave = new Wave(this.signals, 0, this.g, this.metrics, this.clippath);
        this.grid = new Grid(this.g, this.metrics, this.labels)
        this.plot();
    }
    this.setScaleAutoY = setScaleAutoY;
    
    function resetScales()
    {
        this.svg.removeChild(this.g);
	this.zoomRect = undefined;
	this.crosshairVertLine = undefined;
	this.crosshairHorLine = undefined;
        this.g = document.createElementNS("http://www.w3.org/2000/svg","g");
        this.g.setAttribute("id","viewport");
        this.svg.appendChild(this.g);
        var xmin, xmax, ymin, ymax;
        if(this.limits.xMin != undefined)
            xmin = this.limits.xMin;
        else
            xmin = this.xMin;
        if(this.limits.xMax != undefined)
            xmax = this.limits.xMax;
        else
            xmax = this.xMax;
        if(this.limits.yMin != undefined)
            ymin = this.limits.yMin;
        else
            ymin = this.yMin;
        if(this.limits.yMax != undefined)
            ymax = this.limits.yMax;
        else
            ymax = this.yMax;
        this.metrics = new Metrics(METRICS_BORDER, this.metrics.width, this.metrics.height, xmin, xmax, ymin, ymax);
        this.wave = new Wave(this.signals, 0, this.g, this.metrics, this.clippath);
        this.grid = new Grid(this.g, this.metrics, this.labels)
        this.plot();
    }
    this.resetScales = resetScales;

}


function updateSuccess(panelIdx, signals, labels, limits)
{
    var wavePanel;
    wavePanel = wavePanels[panelIdx];
    wavePanel.setPanel(signals, labels, limits);
}

function updateFailure(col, row, error)
{
    alert('Error getting signals for panel ('+row+', '+col+'): '+error);
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
    resizeWaves(window.innerWidth-40,window.innerHeight-100);
}

function eventUpdate(event,response,panel) {
  panel.update();
  panel.plot();
}

function updateTitle(expression) {
  var req = new XMLHttpRequest();
  req.url = 'Scope?title='+expression;
  req.open('GET',req.url,true);
  req.responseType='text';
  req.onreadystatechange = function() {
    if (this.readyState == 4) {
       if (this.status == 200) document.title=decodeUrl(this.response);
       this.onreadystatechange = function() {};
    }
  };
  req.send();
}
    
function eventUpdateFail(event,response,panel) {}

function mdsplusEvent(event,repeat,success_cb,error_cb,userarg) {
  var req = new XMLHttpRequest();
  req.url='event/'+event;
  req.open('GET',req.url,true);
  req.responseType = 'text';
  req.scb=success_cb;
  req.fcb=error_cb;
  req.event=event;
  req.userarg=userarg;
  req.repeat=repeat;
  req.onreadystatechange = function() {
    if (this.readyState == 4) {
       if (this.status == 200) {
          this.scb(this.event,this.response,this.userarg);
          if (this.repeat != 0) {
             this.open('GET',this.url,true);
             this.send();
             return;
          }
       }
       else if (req.status == 204) {
          this.open('GET',this.url,true);
          this.send();
          return;
       } else if (req.status != 0) {
          this.fcb(this.event,req.response,this.userarg);
       }
       this.onreadystatechange = function() {}
    }
  };
  req.send();
  return req;
}

function mdsScopePanel(div,width,height,numCols, numRows, col, row, tree,shot,exprArray, colors, modes, limits, labels) {
  var svg=document.createElementNS("http://www.w3.org/2000/svg","svg");
  
  //svg.setAttribute("ontouchstart", "alert('touchstart');");
    svg.addEventListener('touchstart', function(evt) {
        touchStart(evt);
    }, false);
    svg.addEventListener('touchmove', function(evt) {
        touchMove(evt);
    }, false);
    svg.addEventListener('touchend', function(evt) {
        touchEnd(evt);
    }, false);
    svg.setAttribute("version","1.1");
    svg.setAttributeNS(null,"preserveAspectRatio","none");
    svg.setAttribute("baseProfile","full");
    svg.setAttributeNS(null,"width",width);
    svg.setAttributeNS(null,"height",height);
    svg.setAttributeNS(null,"animatable","no");
    svg.setAttribute("onmousedown", "return mouseDown(evt)");
//    svg.setAttribute("onmousemove", "mouseMove(evt)");
    svg.setAttribute("onmouseup", "return mouseUp(evt)");
    //svg.setAttribute("onmouseclick", "return mouseClick(evt)");
    svg.setAttribute("ontouchmove", "mouseMove(evt)");
    svg.setAttribute("ontouchup", "mouseUp(evtt)");
    svg.setAttribute("viewBox","0 0 "+width +" " +height);
    var clippath=document.createElementNS("http://www.w3.org/2000/svg","clipPath");
    clippath.setAttribute("id","clippath"+Math.random());
    var path=document.createElementNS("http://www.w3.org/2000/svg","path");
    path.setAttribute("d","M 0 0 0 " + height + " " + width + " " + height + " " + width + " 0");
    clippath.appendChild(path);
    svg.appendChild(clippath);
    div.appendChild(svg);

    wavePanel = new WavePanel(svg, wavePanels.length, numCols, numRows, col, row, clippath, tree, shot, colors, modes, exprArray, limits, labels);
    wavePanels.push(wavePanel);
    return wavePanel;
 }

function updateGlobalShots()
{
    var shotList = document.getElementById('GlobalShots').value;
    if(shotList == undefined)
        return;
    
    //First step: erase scopePanels
    
    var shots = shotList.trim().split(',');
    for(var panelIdx = 0; panelIdx < wavePanels.length; panelIdx++)
    {
        wavePanels[panelIdx].shots = shots;
        wavePanels[panelIdx].update();
    }
}

function titleEvent(event,response,expression) {
  updateTitle(expression);
}

function titleEventFailure(event,response,expression) {}

function mdsScope(xmlDoc)
{
    document.oncontextmenu = contextMenu;
    var titleXml = xmlDoc.getElementsByTagName('title');
    if (titleXml.length != 0)
    {
      var expressionXml=titleXml[0].getElementsByTagName('expression');
      var expression=expressionXml[0].childNodes[0].nodeValue;
      updateTitle(expression);
      var eventXml=titleXml[0].getElementsByTagName('event');
      if (eventXml.length != 0)
      {
        var event=eventXml[0].childNodes[0].nodeValue;
        mdsplusEvent(event,true,titleEvent,titleEventFailure,expression);
      }
    }
//Get Color palette

    var paletteXml = xmlDoc.getElementsByTagName('palette');
    if(paletteXml.length != 0)
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
            htmlText += '<tr><td style="height:100%" id="scope_'+rowIdx+'_'+colIdx+'">'+'</td></tr>';
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
            if ( experiment != null ) {
              var shot = panels[panelIdx].getAttribute("shot");
              if (shot != null) {
                var shots = new Array();
                shots.push(parseInt(shot));
              }
            }
            var signals = panels[panelIdx].getElementsByTagName("signal");
            var colors = new Array();
            var modes = new Array();
            var expressions = new Array();
            var limits = new Object();
            var setting;
	    var event=null;
            var panel=panels[panelIdx];
            if((setting=panel.getAttribute('xmin')) != null)
                limits.xMin = setting;
            if((setting=panel.getAttribute('xmax')) != null)
                limits.xMax = setting;
            if((setting=panel.getAttribute('ymin')) != null)
                limits.yMin = setting;
            if((setting=panel.getAttribute('ymax')) != null)
                limits.yMax = setting;
            if((setting=panel.getAttribute('event')) != null)
	        event=setting;
            var labels = new Object();
            {
                if((setting=panel.getAttribute('title')) != null)
                    labels.title = setting;
                if((setting=panel.getAttribute('xlabel')) != null)
                    labels.xLabel = setting;
                if((setting=panel.getAttribute('ylabel')) != null)
                    labels.yLabel = setting;
            }
            for(var signalIdx = 0; signalIdx < signals.length; signalIdx++)
            {
                var color;
                if((setting=signals[signalIdx].getAttribute('color')) != null)
                    color = setting;
                else
                    color = 0;
                colors.push(this.colorPalette[color % this.colorPalette.length]);
                var mode;
                if((setting=signals[signalIdx].getAttribute('mode')) != null)
                    mode = parseInt(setting);
                else
                    mode = PLOT_LINE;
                modes.push(mode);
                var expression = signals[signalIdx].childNodes[0].nodeValue;
                expressions.push(expression);
            }
            var scopePanel=mdsScopePanel(document.getElementById('scope_'+panelIdx+'_'+colIdx),Math.round((window.innerWidth-40) /columns.length),
                    Math.round((window.innerHeight-100) /panels.length),columns.length, panels.length, colIdx, panelIdx, experiment,shots,
                    expressions, colors, modes, limits, labels);
            if (event)
               mdsplusEvent(event,true,eventUpdate,eventUpdateFail,scopePanel);
    
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
                //alert(this.response);
                var isConfig = this.getResponseHeader('IS_CONFIG');
                if(isConfig == 'YES')
                    mdsScope(this.responseXML);
                else
                    document.getElementById("ScopeArea").innerHTML = this.response;
            }
            //this.onreadystatechange = function() {};
        }
    };
    req.send();
}
