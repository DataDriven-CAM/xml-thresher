<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet  xmlns:functx="http://www.functx.com" xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:math="http://www.w3.org/2005/xpath-functions/math" xmlns:xlink="http://www.w3.org/1999/xlink" version="3.0">
    <xsl:output method="text" encoding="UTF-8" indent="yes"/>
    <xsl:strip-space elements="*"/>
    <xsl:template match="/">data_<xsl:value-of select="./PDBx:datablock@datablockName"/>

    </xsl:template>
</xsl:stylesheet>    

