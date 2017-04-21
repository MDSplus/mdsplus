<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" 
xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:psic="http://www.iter.org/CODAC/PlantSystemIandC/2014-2"
xmlns:ps="http://www.iter.org/CODAC/PlantSystem/2014"
exclude-result-prefixes="psic ps"
>

<!--
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
xmlns:psic="http://www.iter.org/CODAC/PlantSystemIandC/2013" 
xmlns:pbs="http://www.iter.org/CODAC/PBS/2012" 
xmlns:epics="http://www.iter.org/CODAC/EPICS/2013" 
xmlns:beast="http://www.iter.org/CODAC/BEAST/2012" 
xmlns:ps="http://www.iter.org/CODAC/PlantSystem/2013" 
xmlns:adm="http://www.iter.org/CODAC/Admin/2012" 
xmlns:icp="http://www.iter.org/CODAC/ICP/2012" 
xmlns:linux="http://www.iter.org/CODAC/Linux/2013" 
xmlns:codac="http://www.iter.org/CODAC/CODACSystem/2013" 
xmlns:ns10="http://www.iter.org/CODAC/BEAUTY/2012" 
xmlns:ns11="http://www.iter.org/CODAC/BOY/2012">
<xsl:template match="psic:variable">
<xsl:value-of select="/psic:IandCProject/psic:subsystems/psic:subsystem/psic:plantSystemIandCs/psic:plantSystemIandC/psic:variables"/>
-->

<xsl:output method="text" version="1.0" encoding="UTF-8" indent="no" omit-xml-declaration="yes" />

<xsl:template match="text()|@*">
</xsl:template>

<xsl:template match="//psic:IandCProject//ps:signals">
<xsl:for-each select = "ps:signal">
   <xsl:sort select = "@name"/>
   <xsl:value-of select = "@name"/>
   <xsl:text> ; </xsl:text>
   <xsl:value-of select = "ps:deployedOn/ps:IOBoard"/>
   <xsl:text> ; </xsl:text>
   <xsl:value-of select = "ps:deployedOn/ps:IOBoardChannel"/>
   <xsl:text>&#10;</xsl:text>
</xsl:for-each>
</xsl:template>

</xsl:stylesheet>
