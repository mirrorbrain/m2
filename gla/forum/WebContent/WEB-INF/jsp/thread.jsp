<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@page import="fr.acceis.forum.bean.ThreadBean"%>
<%@page import="fr.acceis.forum.bean.PostBean"%>
<%@page import="fr.acceis.forum.db.Utils"%>
<html xmlns="http://www.w3.org/1999/xhtml" dir="ltr" xml:lang="en-gb" lang="en-gb">

<head>
	<% ThreadBean t = (ThreadBean) request.getSession().getAttribute("thread"); %>
	<title>
		<% t.getTitle(); %>
	</title>

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

			<table cellspacing="1" width="100%">
				<tbody>
					<tr>
						<td valign="middle" align="left" colspan="4" nowrap="nowrap">
							<a href="create">
								<img src="fichiers/button_topic_new.gif" alt="Post new topic" title="Post new topic" />
							</a>&nbsp;
						</td>
					</tr>
				</tbody>
			</table>

			<br clear="all" />

			<table class="tablebg" cellspacing="1" width="100%">
				<%
					for (PostBean p : t.getPosts())
						out.print("<tbody><tr class=\"row2\"><td valign=\"middle\" align=\"center\"><b class=\"postauthor\">" + p.getAuthor()
							+ "</b></td><td width=\"100%\" height=\"25\"><table cellspacing=\"0\" width=\"100%\"><tbody><tr><td class=\"gensmall\" width=\"100%\"><div style=\"float: left;\">&nbsp;<b>Fil de discussion :</b> "
							+ t.getTitle() + "</div><div style=\"float: right;\"><b>Posted:</b> " + p.getPosted().toString()
							+ "&nbsp;</div></td></tr></tbody></table></td></tr></tbody><tr class=\"row2\"><td class=\"profile\" valign=\"top\"><table cellspacing=\"4\" align=\"center\" width=\"150\"></table> <span class=\"postdetails\">"
							+ " <b>Posts:</b> " + Utils.getNumberOfPosts(p.getAuthor()) + "</span></td><td valign=\"top\"><table cellspacing=\"5\" width=\"100%\"><tbody><tr><td><div class=\"postbody\">" + p.getPost()
							+ "</div><br clear=\"all\" /><br /><table cellspacing=\"0\" width=\"100%\"><tbody><tr valign=\"middle\"><td class=\"gensmall\" align=\"right\"></td></tr></tbody></table></td></tr></tbody></table></td></tr>"
							+ "<tr><td class=\"spacer\" colspan=\"2\" height=\"1\"><img src=\"fichiers/spacer.gif\" alt=\"\" width=\"1\" height=\"1\" /></td></tr>");
				%>
			</table>
		</div>

		<br clear="all" />

		<form action="#" method="post">
			<table class="tablebg" cellspacing="1" width="100%">
				<tbody>
					<tr>
						<th colspan="2">R&eacute;ponse</th>
					</tr>
		
					<tr>
						<td class="row2">
							<table style="width: 100%;" cellspacing="1" cellpadding="4" align="center">
								<tbody>
									<tr>
										<td valign="top">
											<b class="gensmall">Message :</b>
										</td>
										<td>
											<textarea class="post" name="message" cols="100%" rows="5" tabindex="2" style="resize:none"></textarea>
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