from html.parser import HTMLParser
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[1]


class LessonParser(HTMLParser):
    def __init__(self):
        super().__init__()
        self.ids = set()
        self.links = []
        self.scripts = []

    def handle_starttag(self, tag, attrs):
        values = dict(attrs)
        if "id" in values:
            self.ids.add(values["id"])
        if tag == "a" and "href" in values:
            self.links.append(values["href"])
        if tag == "script" and "src" in values:
            self.scripts.append(values["src"])


class LessonFiveStructureTest(unittest.TestCase):
    def setUp(self):
        self.lesson_path = ROOT / "lessons/0005-review-the-architecture.html"
        self.reference_path = ROOT / "reference/architecture-review.html"
        self.lesson = self.lesson_path.read_text(encoding="utf-8")
        self.reference = self.reference_path.read_text(encoding="utf-8")

    def test_lesson_has_three_feedback_loops_and_shared_assets(self):
        parser = LessonParser()
        parser.feed(self.lesson)
        self.assertTrue({"review", "sort", "quiz"}.issubset(parser.ids))
        self.assertIn("../assets/quiz.js", parser.scripts)
        self.assertIn("../assets/categorize.js", parser.scripts)
        self.assertIn("../assets/course.css", self.lesson)

    def test_lesson_covers_the_four_architectural_views(self):
        for term in ("Hexagonal", "Clean", "Onion", "传统分层"):
            self.assertIn(term, self.lesson)

    def test_lesson_links_primary_sources_and_reference(self):
        parser = LessonParser()
        parser.feed(self.lesson)
        required = {
            "https://alistair.cockburn.us/hexagonal-architecture",
            "https://blog.cleancoder.com/uncle-bob/2012/08/13/the-clean-architecture.html",
            "https://jeffreypalermo.com/2008/07/the-onion-architecture-part-1/",
            "https://martinfowler.com/bliki/PresentationDomainDataLayering.html",
            "../reference/architecture-review.html",
        }
        self.assertTrue(required.issubset(set(parser.links)))

    def test_reference_and_index_are_connected(self):
        index = (ROOT / "index.html").read_text(encoding="utf-8")
        self.assertIn("lessons/0005-review-the-architecture.html", index)
        self.assertIn("reference/architecture-review.html", index)
        self.assertIn("0005-review-the-architecture.html", self.reference)

    def test_resources_record_all_primary_sources(self):
        resources = (ROOT / "RESOURCES.md").read_text(encoding="utf-8")
        for url in (
            "https://blog.cleancoder.com/uncle-bob/2012/08/13/the-clean-architecture.html",
            "https://jeffreypalermo.com/2008/07/the-onion-architecture-part-1/",
            "https://martinfowler.com/bliki/PresentationDomainDataLayering.html",
        ):
            self.assertIn(url, resources)


if __name__ == "__main__":
    unittest.main()
