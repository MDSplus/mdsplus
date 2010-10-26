<xsl:stylesheet xmlns:yaslt="http://www.mod-xslt2.com/ns/1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xs="http://www.w3.org/2001/XMLSchema" version="1.0" extension-element-prefixes="yaslt"
  xmlns:fn="http://www.w3.org/2005/xpath-functions">
<xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes"/>

<xsl:template match="engineconfig">
    <tree>
  <xsl:choose>
    <xsl:when test="write_period">
      <member NAME="WRITE_PERIOD" USAGE="NUMERIC">
        <data><xsl:value-of select="write_period"/></data>
      </member>
    </xsl:when>
    <xsl:otherwise>
      <member NAME="WRITE_PERIOD" USAGE="NUMERIC">
        <data>30</data>
      </member>
    </xsl:otherwise>
  </xsl:choose>
  
  <xsl:choose>
    <xsl:when test="get_threshold">
      <member NAME="GET_TRESH" USAGE="NUMERIC">
       <data><xsl:value-of select="get_threshold"/></data>
      </member>
    </xsl:when>
    <xsl:otherwise>
      <member NAME="GET_TRESH" USAGE="NUMERIC">
        <data>20</data>
      </member>
    </xsl:otherwise>
  </xsl:choose>

  <xsl:choose>
    <xsl:when test="file_size">
      <member NAME="FILE_SIZE" USAGE="NUMERIC">
       <data><xsl:value-of select="file_size"/></data>
      </member>
    </xsl:when>
    <xsl:otherwise>
      <member NAME="FILE_SIZE" USAGE="NUMERIC">
        <data>100</data>
      </member>
    </xsl:otherwise>
  </xsl:choose>

  <xsl:choose>
    <xsl:when test="max_repeat_count">
      <member NAME="MAX_REPEAT" USAGE="NUMERIC">
       <data><xsl:value-of select="max_repeat_count"/></data>
      </member>
    </xsl:when>
    <xsl:otherwise>
      <member NAME="MAX_REPEAT" USAGE="NUMERIC">
        <data>120</data>
      </member>
    </xsl:otherwise>
  </xsl:choose>

  <xsl:choose>
    <xsl:when test="ignored_future">
      <member NAME="IGN_FUTURE" USAGE="NUMERIC">
       <data><xsl:value-of select="ignored_future"/></data>
      </member>
    </xsl:when>
    <xsl:otherwise>
      <member NAME="IGN_FUTURE" USAGE="NUMERIC">
        <data>120</data>
      </member>
    </xsl:otherwise>
  </xsl:choose>

<xsl:apply-templates select="group"/>
   </tree>
</xsl:template>   


<xsl:template match="group">  
     <node> 	
     <xsl:attribute name="NAME">
     	<xsl:value-of select="name"/>
     </xsl:attribute>
     <member NAME="DISABLE" USAGE="NUMERIC">
     <xsl:choose>
       <xsl:when test="channel/disable">
       	  <data>_<xsl:value-of select="translate(channel/disable[1]/../name, ':', '_')"/>  &lt; 0</data> 
       </xsl:when>
     <xsl:otherwise>
     	<data>0</data>
     </xsl:otherwise>
     </xsl:choose>
     </member>
     <xsl:apply-templates select="channel"/>
     </node>
 </xsl:template>
 
 
 <xsl:template match="channel">
  <node>
     <xsl:attribute name="NAME">
<!--     	<xsl:value-of select="substring(translate(name, ':', '_'), string-length(name) - 11)"/> -->
     	<xsl:value-of select="translate(name, ':', '_')"/>
     </xsl:attribute>
     <xsl:attribute name="TAGS">
     	<xsl:value-of select="translate(name, ':', '_')"/>
     </xsl:attribute>
  <member NAME="EGU" USAGE="TEXT"/>
  <member NAME="HOPR" USAGE="NUMERIC"/>
  <member NAME="LOPR" USAGE="NUMERIC"/>
  <member NAME="PERIOD" USAGE="NUMERIC">
     <data><xsl:value-of select="period"/></data>
  </member>  
  <member NAME="REC_NAME" USAGE="TEXT">
     <data>"<xsl:value-of select="name"/>"</data>
  </member>
  <member NAME="SCAN_MODE" USAGE="TEXT">
  <xsl:choose>
    <xsl:when test="monitor">
      <data>"MONITOR"</data>
    </xsl:when>
    <xsl:otherwise>
      <data>"SCAN"</data>
    </xsl:otherwise>
  </xsl:choose>
  </member>
   <member NAME="IS_DISABLE" USAGE="TEXT">
  <xsl:choose>
    <xsl:when test="disable">
      <data>"YES"</data>
    </xsl:when>
    <xsl:otherwise>
      <data>"NO"</data>
    </xsl:otherwise>
  </xsl:choose>
  </member>
  <member NAME="VAL" USAGE="SIGNAL"/>
  <member NAME="ALARM" USAGE="SIGNAL"/>
  </node>
</xsl:template>

 
 
 
</xsl:stylesheet> 
  
