<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet  xmlns:functx="http://www.functx.com" xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:math="http://www.w3.org/2005/xpath-functions/math" xmlns:xlink="http://www.w3.org/1999/xlink" version="3.0">
    <xsl:output method="xml" encoding="UTF-8" indent="yes"/>
    <xsl:strip-space elements="*"/>
    <xsl:template match="/">
<xsl:apply-templates select="note"/>

    </xsl:template>
    <xsl:template match="note"><xsl:text> </xsl:text><xsl:value-of select="./me"/><xsl:text> </xsl:text><xsl:value-of select="./from"/><xsl:text> "</xsl:text><xsl:value-of select="./body"/><xsl:text>"</xsl:text>
    </xsl:template>
</xsl:stylesheet>    

