/*
 * The GNU General Public License does not permit incorporating your program
 * into proprietary programs. If your program is a subroutine library, you may
 * consider it more useful to permit linking proprietary applications with the
 * library. If this is what you want to do, use the GNU Lesser General Public
 * License instead of this License. But first, please read
 * <http://www.gnu.org/philosophy/why-not-lgpl.html>.
 * ----------------------------------------------------------------------------
 * This source file is part of a university project and belongs to its creator
 * and the university it was made for.
 * @author Maël Nogues mael.nogues@outlook.com
 */
package fr.acceis.forum.servlet;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

import org.apache.tomcat.util.security.Escape;

import fr.acceis.forum.db.Utils;

/**
 * The Class ThreadServlet.
 */
public class CreateServlet extends HttpServlet {

	/*
	 * (non-Javadoc)
	 * @see javax.servlet.http.HttpServlet#doGet(javax.servlet.http.
	 * HttpServletRequest, javax.servlet.http.HttpServletResponse)
	 */
	@Override
	protected void doGet(final HttpServletRequest req, final HttpServletResponse resp)
			throws ServletException, IOException {
		final HttpSession s = req.getSession();
		final String u = (String) s.getAttribute("user");
		if (u != null && u != "")
			req.getRequestDispatcher("/WEB-INF/jsp/create.jsp").forward(req, resp);
		else
			resp.sendRedirect("home");
	}

	/*
	 * (non-Javadoc)
	 * @see javax.servlet.http.HttpServlet#doPost(javax.servlet.http.
	 * HttpServletRequest, javax.servlet.http.HttpServletResponse)
	 */
	@Override
	protected void doPost(final HttpServletRequest req, final HttpServletResponse resp)
			throws ServletException, IOException {
		final String title = Escape.htmlElementContent(Escape.xml(req.getParameter("title"))),
				post = Escape.htmlElementContent(Escape.xml(req.getParameter("message"))).replace("\n", "<br />");

		final long id = Utils.newThread(title, post, (String) req.getSession().getAttribute("user"));

		resp.sendRedirect("thread?id=" + id);
	}

}
