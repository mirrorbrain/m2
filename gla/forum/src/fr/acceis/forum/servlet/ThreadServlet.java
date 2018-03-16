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
import java.util.ArrayList;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

import org.apache.tomcat.util.security.Escape;

import fr.acceis.forum.bean.ThreadBean;
import fr.acceis.forum.db.Utils;

/**
 * The Class ThreadServlet.
 */
public class ThreadServlet extends HttpServlet {

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
		if (u != null && u != "") {
			final ArrayList<ThreadBean> tl = (ArrayList<ThreadBean>) s.getAttribute("threads");
			final long id = Long.valueOf(req.getParameter("id"));

			for (final ThreadBean t : tl)
				if (t.getId() == id) {
					s.setAttribute("thread", t);
					break;
				}
			if (s.getAttribute("thread") != null)
				req.getRequestDispatcher("/WEB-INF/jsp/thread.jsp").forward(req, resp);
			else
				resp.sendRedirect("home");
		} else
			resp.sendRedirect("login");
	}

	/*
	 * (non-Javadoc)
	 * @see javax.servlet.http.HttpServlet#doPost(javax.servlet.http.
	 * HttpServletRequest, javax.servlet.http.HttpServletResponse)
	 */
	@Override
	protected void doPost(final HttpServletRequest req, final HttpServletResponse resp)
			throws ServletException, IOException {
		final HttpSession s = req.getSession();
		final String post = Escape.htmlElementContent(Escape.xml(req.getParameter("message"))).replace("\n", "<br />");
		final long id = Long.valueOf(req.getParameter("id"));

		Utils.newPost(id, ((ThreadBean) s.getAttribute("thread")).getTitle(), post, (String) s.getAttribute("user"));
		req.getSession().setAttribute("threads", Utils.getThreads());
		doGet(req, resp);
	}

}
