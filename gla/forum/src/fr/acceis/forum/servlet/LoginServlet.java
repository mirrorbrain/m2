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

import fr.acceis.forum.db.Utils;

/**
 * The Class LoginServlet.
 */
public class LoginServlet extends HttpServlet {

	/*
	 * (non-Javadoc)
	 * @see javax.servlet.http.HttpServlet#doGet(javax.servlet.http.
	 * HttpServletRequest, javax.servlet.http.HttpServletResponse)
	 */
	@Override
	protected void doGet(final HttpServletRequest req, final HttpServletResponse resp)
			throws ServletException, IOException {
		if (req.getAttribute("failed") == null)
			req.setAttribute("failed", false);
		req.getRequestDispatcher("/WEB-INF/jsp/login.jsp").forward(req, resp);
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
		final String login = req.getParameter("username"), password = req.getParameter("password");
		final HttpSession s = req.getSession();

		if (Utils.connectUser(login, password)) {
			s.setAttribute("user", login);
			resp.sendRedirect("home");
		} else {
			s.removeAttribute("user");
			req.setAttribute("failed", true);
			doGet(req, resp);
		}

	}

}
