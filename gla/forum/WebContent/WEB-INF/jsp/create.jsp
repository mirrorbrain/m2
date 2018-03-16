<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" dir="ltr" xml:lang="en-gb" lang="en-gb">

<head>
	<title>new thread</title>

	<link rel="stylesheet" href="fichiers/style.css" type="text/css" />
</head>

<body class="ltr">

	<div id="wrapcentre">
		<div id="pagecontent">
			<table class="tablebg" style="margin-top: 5px;" cellspacing="1" cellpadding="0" width="100%">
				<tbody>
					<tr>
						<td class="row1">
							<p class="breadcrumbs">
								<%
									final String login = (String) request.getSession().getAttribute("user");
									if (login != null && login != "")
										out.print("Bonjour " + login + "\t<a href=\"logout\">D&eacute;connexion</a>");
									else
										out.print("<a href=\"login\">Connexion</a> / <a href=\"user\">Inscription</a>");
								%>
							</p>
						</td>
					</tr>
				</tbody>
			</table>
			<br clear="all" />

			<form action="#" method="post">
				<table class="tablebg" cellspacing="1" width="100%">
					<tbody>
						<tr>
							<th colspan="2">Thread</th>
						</tr>

						<tr>
							<td class="row2">
								<table style="width: 100%;" cellspacing="1" cellpadding="4" align="center">
									<tbody>
										<tr>
											<td valign="top">
												<b class="gensmall">Titre :</b>
											</td>
											<td>
												<input class="post" name="title" size="25" tabindex="1" type="text" />
											</td>
										</tr>
										<tr>
											<td valign="top">
												<b class="gensmall">Message :</b>
											</td>
											<td>
												<textarea class="post" name="message" cols="50" rows="5" tabindex="2" style="resize:none"></textarea>
											</td>
										</tr>
									</tbody>
								</table>
							</td>
						</tr>

						<tr>
							<td class="cat" colspan="2" align="center">
								<input name="create" class="btnmain" value="Cr&eacute;er" tabindex="5" type="submit" />
							</td>
						</tr>
					</tbody>
				</table>
			</form>
		</div>

		<table class="tablebg" style="margin-top: 5px;" cellspacing="1" cellpadding="0" width="100%">
			<tbody>
				<tr>
					<td class="row1">
						<p class="breadcrumbs">
							<a href="home">Index du forum</a>
						</p>
					</td>
				</tr>
			</tbody>
		</table>
	</div>
</body>

</html>