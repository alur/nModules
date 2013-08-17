<?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:preserve-space elements="code blockcode"/>
  <xsl:output method="html"/>
  <xsl:param name="local_extension">xml</xsl:param>

  <xsl:template match="/">
    <html>
      <head>
        <title>
          <xsl:value-of select="//title"/>
        </title>
        <link href="nDocs.css" rel="stylesheet" type="text/css"/>
      </head>
      <body>
        <xsl:apply-templates/>
      </body>
    </html>
  </xsl:template>

  <!-- Page -->
  <xsl:template match="page">
    <h1>
      <xsl:value-of select="title"/>
    </h1>
    <hr/>
    <xsl:apply-templates select="tableofcontents"/>
    <div class="description">
      <xsl:value-of select="description"/>
    </div>
    <xsl:apply-templates select="section"/>
  </xsl:template>
  
  <!-- Section -->
  <xsl:template match="section">
    <h2 id="{title}">
      <xsl:value-of select="title"/>
    </h2>
    <hr/>
    <div class="section">
      <xsl:apply-templates select="description"/>
      <xsl:apply-templates select="setting"/>
    </div>
  </xsl:template>
  
  <!-- Setting -->
  <xsl:template match="setting">
    <h3 id="{name}">
      <xsl:value-of select="name"/>
      <span class="type">
        [<xsl:value-of select="type"/>]
      </span>
    </h3>
    <div class="setting">
      <xsl:apply-templates select="description"/>
      <xsl:apply-templates select="enumeration"/>
      <xsl:apply-templates select="bangs"/>
      <xsl:apply-templates select="scripting"/>
      <xsl:apply-templates select="default"/>
      <xsl:apply-templates select="note"/>
    </div>
  </xsl:template>

  <!-- Description -->
  <xsl:template match="description">
    <div class="description">
      <xsl:apply-templates/>
    </div>
  </xsl:template>
  
  <!-- Bangs -->
  <xsl:template match="bangs">
    <h4>
      Bangs
    </h4>
    <div class="bangs">
      <xsl:apply-templates/>
    </div>
  </xsl:template>

  <xsl:template match="bang">
    <span class="bang">
      <b>
        !<xsl:value-of select="name"/>
      </b>
      <xsl:for-each select="parameters/parameter">
        <xsl:text> </xsl:text>
        <i title="{description}&#10;Type: {type}">
          <xsl:value-of select="name"/>
        </i>
      </xsl:for-each>
    </span>
  </xsl:template>
  
  <!-- Scripting -->
  <xsl:template match="scripting">
    <h4>
      Scripting
    </h4>
    <div class="scripting">
      <xsl:apply-templates/>
    </div>
  </xsl:template>
  
  <!-- Default -->
  <xsl:template match="default">
    <h4>
      Default Value
    </h4>
    <span class="default-value {@class}">
      <xsl:value-of select="."/>
    </span>
    <br/>
  </xsl:template>
    
  <!-- Enumeration -->
  <xsl:template match="enumeration">
    <h4>
      <xsl:value-of select="title"/>
    </h4>
    <div class="enumeration">
      <xsl:for-each select="value">
        <h5>
          <xsl:value-of select="name"/>
        </h5>
        <div class="description">
          <xsl:value-of select="description"/>
        </div>
      </xsl:for-each>
    </div>
  </xsl:template>
  
  <!-- Note -->
  <xsl:template match="note">
    <div class="note">
      <b>
        Note: 
      </b>
      <span class="note {@class}">
        <xsl:value-of select="."/>
      </span>
    </div>
  </xsl:template>
  
  <!-- Table of contents -->
  <xsl:template match="tableofcontents">
    <div class="tableofcontents">
      <h4>
        Table of Contents
      </h4>
      <ol>
        <xsl:for-each select="/page/section">
          <li>
            <a href="#{title}">
              <xsl:value-of select="title"/>
            </a>
          </li>
        </xsl:for-each>
      </ol>
    </div>
  </xsl:template>
  
  
  <xsl:template match="const | scriptfunc | bangcommand">
    <span class="{local-name(.)}">
      <xsl:apply-templates/>
    </span>
  </xsl:template>

</xsl:stylesheet>

