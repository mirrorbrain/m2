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
 * The Class LoginServlet.
 */
public class UserServlet extends HttpServlet {

	/*
	 * (non-Javadoc)
	 * @see javax.servlet.http.HttpServlet#doGet(javax.servlet.http.
	 * HttpServletRequest, javax.servlet.http.HttpServletResponse)
	 */
	@Override
	protected void doGet(final HttpServletRequest req, final HttpServletResponse resp)
			throws ServletException, IOException {
		if (req.getAttribute("failed-on-login") == null)
			req.setAttribute("failed-on-login", false);
		if (req.getAttribute("failed") == null)
			req.setAttribute("failed", false);
		req.getRequestDispatcher("/WEB-INF/jsp/user.jsp").forward(req, resp);
	}

	/*
	 * (non-Javadoc)
	 * @see javax.servlet.http.HttpServlet#doPost(javax.servlet.http.
	 * HttpServletRequest, javax.servlet.http.HttpServletResponse)
	 */
	@Override
	protected void doPost(final HttpServletRequest req, final HttpServletResponse resp)
			throws ServletException, IOException {
		// doGet(req, resp);
		final String login = Escape.htmlElementContent(Escape.xml(req.getParameter("username"))),
				password = req.getParameter("password"), password_conf = req.getParameter("password-conf"),
				mail = Escape.htmlElementContent(Escape.xml(req.getParameter("mail")));
		final HttpSession s = req.getSession();

		if (Utils.exists(login)) {
			req.setAttribute("failed-on-login", true);
			doGet(req, resp);
			return;
		}

		if (password.equals(password_conf) && Utils.addUser(login, password, mail)) {
			s.setAttribute("user", login);
			resp.sendRedirect("home");
		} else {
			s.removeAttribute("user");
			req.setAttribute("failed", true);
			doGet(req, resp);
		}

	}

}
