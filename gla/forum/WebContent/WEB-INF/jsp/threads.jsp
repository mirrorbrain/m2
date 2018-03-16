<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@page import="java.util.ArrayList"%>
<%@page import="fr.acceis.forum.bean.ThreadBean"%>
<html xmlns="http://www.w3.org/1999/xhtml" dir="ltr" xml:lang="en-gb" lang="en-gb">

<head>
	<title>forum</title>

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
						<td valign="middle" align="left">
							<a href="create">
								<img src="fichiers/button_topic_new.gif" alt="Post new topic" title="Post new topic" />
							</a>
						</td>
					</tr>
				</tbody>
			</table>

			<br clear="all" />

			<table class="tablebg" cellspacing="1" width="100%">
				<tbody>
					<tr>
						<td class="cat" colspan="4">
							<table cellspacing="0" width="100%">
								<tbody>
									<tr class="nav">
										<td valign="middle">&nbsp;</td>
										<td valign="middle" align="right">&nbsp;</td>
									</tr>
								</tbody>
							</table>
						</td>
					</tr>

					<tr>
						<th>&nbsp;Topics&nbsp;</th>
						<th>&nbsp;Auteur&nbsp;</th>
						<th>&nbsp;R&eacute;ponses&nbsp;</th>
						<th>&nbsp;Date de cr&eacute;ation&nbsp;</th>
					</tr>
					<%
						final ArrayList<ThreadBean> threads = (ArrayList<ThreadBean>) session.getAttribute("threads");
						for (ThreadBean t : threads)
							out.print("<tr><td class=\"row1\"><a class=\"topictitle\" href=\"thread?id=" + t.getId() + "\" >"
								+ t.getTitle()
								+ "</a></td><td class=\"row2\" align=\"center\" width=\"130\"><p class=\"topicauthor\"><a class=\"username-coloured\">"
								+ t.getAuthor()
								+ "</a></p></td><td class=\"row1\" align=\"center\" width=\"50\"><p class=\"topicdetails\">"
								+ t.getNumberOfPosts()
								+ "</p></td><td class=\"row2\" align=\"center\" width=\"50\"><p class=\"topicdetails\">"
								+ t.getCreated().toString() + "</p></td></tr>");
					%>
				</tbody>
			</table>

			<br clear="all" />
		</div>

	</div>
</body>

</html>