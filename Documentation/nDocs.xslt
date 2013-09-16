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
        <link href="nDocs-light.css" rel="stylesheet" id="style" type="text/css"/>
        <script type="text/javascript" src="nDocs.js"></script>
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
    <xsl:apply-templates select="description"/>
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
      <xsl:apply-templates select="version"/>
      <xsl:apply-templates select="setting"/>
      <xsl:apply-templates select="developers"/>
      <xsl:apply-templates select="bang"/>
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


  <xsl:template match="settingref">
    <a class="settingref" href="#{.}">
      <xsl:value-of select="."/>
    </a>
  </xsl:template>

  <!-- Version -->
  <xsl:template match="version">
    <h3 id="{title}">
      <xsl:value-of select="title"/>
      <span class="type">
        [<xsl:value-of select="date"/>]
      </span>
    </h3>
    <xsl:apply-templates select="changes/*"/>
  </xsl:template>

  <xsl:template match="fixed">
    <b>
      Fixed:
    </b>
    <xsl:value-of select="."/>
    <br />
  </xsl:template>
  <xsl:template match="added">
    <b>
      Added:
    </b>
    <xsl:value-of select="."/>
    <br />
  </xsl:template>
  <xsl:template match="removed">
    <b>
      Removed:
    </b>
    <xsl:value-of select="."/>
    <br />
  </xsl:template>
  <xsl:template match="changed">
    <b>
      Changed:
    </b>
    <xsl:value-of select="."/>
    <br />
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

  <xsl:template match="bangref">
    <a class="bangref" href="#!{.}">
      !<xsl:value-of select="."/>
    </a>
  </xsl:template>

  <!-- Bang -->
  <xsl:template match="bang">
    <h5 id="!{name}">
      !<xsl:value-of select="name"/>
      <span class="bang-parameters">
        <xsl:for-each select="parameters/parameter">
          <xsl:text> </xsl:text>
          <i title="{description}&#10;Type: {type}">
            <xsl:value-of select="name"/>
          </i>
        </xsl:for-each>
      </span>
    </h5>
    <div class="bang">
      <xsl:apply-templates select="description"/>
      <xsl:for-each select="parameters/parameter">
        <h5>
          <xsl:value-of select="name"/>
          <xsl:text> </xsl:text>
          <a class="type" href="Types#{type}">
            [<xsl:value-of select="type"/>]
          </a>
        </h5>
        <div class="bangparamdesc">
          <xsl:apply-templates select="description"/>
        </div>
      </xsl:for-each>
    </div>
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
  <xsl:template match="scriptfunc">
    
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

  <!-- Developers -->
  <xsl:template match="developers">
    <ul>
      <xsl:for-each select="developer">
        <li>
          <xsl:value-of select="name"/> [<xsl:value-of select="nick"/>] <br/>
          Contact: <xsl:value-of select="email"/>
        </li>
      </xsl:for-each>
    </ul>
  </xsl:template>

  <!-- Code -->
  <xsl:template match="code">
    <div class="code">
      <xsl:apply-templates/>
    </div>
  </xsl:template>
  <xsl:template match="multisetting">
    <span class="multi-key">
      *<xsl:value-of select="@name"/>
    </span>
    <xsl:text> </xsl:text>
    <span class="value @{type}">
      <xsl:value-of select="."/>
    </span>
    <br/>
  </xsl:template>
  <xsl:template match="blankrow">
    <br/>
  </xsl:template>
  <xsl:template match="singlesetting">
    <span class="single-key">
      <xsl:value-of select="@name"/>
    </span>
    <xsl:text> </xsl:text>
    <span class="value @{type}">
      <xsl:value-of select="."/>
    </span>
    <br/>
  </xsl:template>
  <xsl:template match="comment">
    <span class="comment">
      ;<xsl:value-of select="."/>
    </span>
    <br/>
  </xsl:template>
  <xsl:template match="group">
    <span class="group-name">
      <xsl:value-of select="@name"/>
    </span>
    <br/>
    {
    <div class="group">
      <xsl:apply-templates/>
    </div>
    }<br/>
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


  <xsl:template match="blockcode">
    <pre class="blockcode">
      <xsl:apply-templates/>
    </pre>
  </xsl:template>

  <xsl:template match="const | scriptfunc | bangcommand">
    <span class="{local-name(.)}">
      <xsl:apply-templates/>
    </span>
  </xsl:template>


  <xsl:template match="img">
    <img src="img/{.}" style="float: right; margin-right: 1em; margin-left: 1em;"/>
  </xsl:template>


  <!-- Supported HTML Tags -->
  <xsl:template match="li | ol | ul | p | b">
    <xsl:copy>
      <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="a">
    <a href="{@href}" target="{@target}">
      <xsl:value-of select="."/>
    </a>
  </xsl:template>

  <!-- JavaScript -->
  <xsl:template match="jscript">
    <script type="text/javascript">
      <xsl:value-of select="."/>
    </script>
  </xsl:template>

  <xsl:template match="placeholder">
    <span id="{.}">
    </span>
  </xsl:template>

</xsl:stylesheet>

